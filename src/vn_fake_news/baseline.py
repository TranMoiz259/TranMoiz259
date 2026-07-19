from __future__ import annotations

import json
from pathlib import Path
from typing import Any, Dict, List

import joblib
import pandas as pd
from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import (
    classification_report,
    confusion_matrix,
    f1_score,
    precision_recall_fscore_support,
)
from sklearn.pipeline import Pipeline


def build_baseline_pipeline() -> Pipeline:
    return Pipeline(
        steps=[
            ("tfidf", TfidfVectorizer(ngram_range=(1, 2), max_features=100000)),
            (
                "clf",
                LogisticRegression(
                    max_iter=1000,
                    class_weight="balanced",
                    n_jobs=-1,
                ),
            ),
        ]
    )


def evaluate(y_true: pd.Series, y_pred: pd.Series) -> Dict[str, Any]:
    labels = sorted(set(y_true) | set(y_pred))
    p, r, f1, support = precision_recall_fscore_support(y_true, y_pred, labels=labels, zero_division=0)
    per_class = [
        {
            "label": label,
            "precision": float(pi),
            "recall": float(ri),
            "f1": float(fi),
            "support": int(si),
        }
        for label, pi, ri, fi, si in zip(labels, p, r, f1, support)
    ]
    return {
        "macro_f1": float(f1_score(y_true, y_pred, average="macro")),
        "per_class": per_class,
        "classification_report": classification_report(y_true, y_pred, zero_division=0),
        "confusion_matrix": confusion_matrix(y_true, y_pred, labels=labels).tolist(),
        "labels": labels,
    }


def train_and_evaluate_baseline(
    train_df: pd.DataFrame,
    test_df: pd.DataFrame,
    artifact_dir: str | Path,
) -> Dict[str, Any]:
    model = build_baseline_pipeline()
    model.fit(train_df["text"], train_df["label"])
    preds = model.predict(test_df["text"])
    results = evaluate(test_df["label"], preds)

    artifact_path = Path(artifact_dir)
    artifact_path.mkdir(parents=True, exist_ok=True)
    joblib.dump(model, artifact_path / "baseline_tfidf_lr.joblib")
    (artifact_path / "baseline_metrics.json").write_text(
        json.dumps(results, ensure_ascii=False, indent=2),
        encoding="utf-8",
    )
    return results
