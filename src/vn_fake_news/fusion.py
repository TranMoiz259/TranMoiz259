from __future__ import annotations

import json
from pathlib import Path
from typing import Dict

import joblib
import numpy as np
import pandas as pd
import torch
from sklearn.compose import ColumnTransformer
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import f1_score, precision_recall_fscore_support
from sklearn.preprocessing import OneHotEncoder, StandardScaler
from transformers import AutoModel, AutoTokenizer

MODEL_NAME = "vinai/phobert-base"


def _metadata_matrix(train_df: pd.DataFrame, test_df: pd.DataFrame):
    transformer = ColumnTransformer(
        transformers=[
            ("cat", OneHotEncoder(handle_unknown="ignore"), ["source_type"]),
            ("num", StandardScaler(), ["engagement"]),
        ]
    )
    train_meta = transformer.fit_transform(train_df)
    test_meta = transformer.transform(test_df)
    return transformer, train_meta, test_meta


def _embed_texts(texts: list[str], batch_size: int = 16) -> np.ndarray:
    tokenizer = AutoTokenizer.from_pretrained(MODEL_NAME)
    model = AutoModel.from_pretrained(MODEL_NAME)
    model.eval()

    all_vecs = []
    for idx in range(0, len(texts), batch_size):
        batch = texts[idx : idx + batch_size]
        inputs = tokenizer(batch, truncation=True, padding=True, max_length=256, return_tensors="pt")
        with torch.no_grad():
            outputs = model(**inputs)
            vectors = outputs.last_hidden_state[:, 0, :].cpu().numpy()
        all_vecs.append(vectors)
    return np.vstack(all_vecs)


def train_fusion_model(
    train_df: pd.DataFrame,
    test_df: pd.DataFrame,
    artifact_dir: str | Path,
) -> Dict[str, float]:
    labels = sorted(train_df["label"].unique())
    label_to_id = {label: idx for idx, label in enumerate(labels)}
    y_train = train_df["label"].map(label_to_id).values
    y_test = test_df["label"].map(label_to_id).values

    metadata_transformer, train_meta, test_meta = _metadata_matrix(train_df, test_df)
    train_text_emb = _embed_texts(train_df["text"].tolist())
    test_text_emb = _embed_texts(test_df["text"].tolist())

    train_features = np.hstack([train_text_emb, train_meta.toarray()])
    test_features = np.hstack([test_text_emb, test_meta.toarray()])

    clf = LogisticRegression(max_iter=1000, class_weight="balanced")
    clf.fit(train_features, y_train)
    preds = clf.predict(test_features)

    p, r, f1, _ = precision_recall_fscore_support(y_test, preds, average="macro", zero_division=0)
    metrics = {
        "macro_f1": float(f1_score(y_test, preds, average="macro")),
        "macro_precision": float(p),
        "macro_recall": float(r),
        "macro_f1_prfs": float(f1),
    }

    out = Path(artifact_dir)
    out.mkdir(parents=True, exist_ok=True)
    joblib.dump({"model": clf, "metadata_transformer": metadata_transformer, "label_to_id": label_to_id}, out / "fusion.joblib")
    (out / "fusion_metrics.json").write_text(json.dumps(metrics, ensure_ascii=False, indent=2), encoding="utf-8")
    return metrics
