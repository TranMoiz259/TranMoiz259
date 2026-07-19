from __future__ import annotations

import re
from pathlib import Path
from typing import Iterable, Tuple

import pandas as pd
from sklearn.metrics import cohen_kappa_score

REQUIRED_COLUMNS = {"text", "label", "published_at", "source_type", "engagement"}


def normalize_text(text: str) -> str:
    text = str(text).strip().lower()
    text = re.sub(r"\s+", " ", text)
    return text


def load_dataset(path: str | Path) -> pd.DataFrame:
    df = pd.read_csv(path)
    missing = REQUIRED_COLUMNS - set(df.columns)
    if missing:
        raise ValueError(f"Missing required columns: {sorted(missing)}")
    df["published_at"] = pd.to_datetime(df["published_at"], errors="coerce", utc=True)
    if df["published_at"].isna().any():
        raise ValueError("published_at contains invalid timestamps")
    df["text"] = df["text"].map(normalize_text)
    df["label"] = df["label"].astype(str).str.lower().str.strip()
    return df


def filter_domain(df: pd.DataFrame, domain: str) -> pd.DataFrame:
    if "domain" not in df.columns:
        return df.copy()
    return df[df["domain"].str.lower() == domain.lower()].copy()


def time_based_split(
    df: pd.DataFrame,
    test_ratio: float = 0.2,
) -> Tuple[pd.DataFrame, pd.DataFrame]:
    if not 0 < test_ratio < 1:
        raise ValueError("test_ratio must be between 0 and 1")
    ordered = df.sort_values("published_at").reset_index(drop=True)
    split_idx = int(len(ordered) * (1 - test_ratio))
    train_df = ordered.iloc[:split_idx].copy()
    test_df = ordered.iloc[split_idx:].copy()
    return train_df, test_df


def create_annotation_template(
    texts: Iterable[str],
    output_path: str | Path,
) -> Path:
    out = Path(output_path)
    out.parent.mkdir(parents=True, exist_ok=True)
    df = pd.DataFrame({"sample_id": range(1, len(list(texts)) + 1), "text": list(texts), "label": ""})
    df.to_csv(out, index=False)
    return out


def compute_cohen_kappa(
    annotator_a_path: str | Path,
    annotator_b_path: str | Path,
) -> float:
    a = pd.read_csv(annotator_a_path)
    b = pd.read_csv(annotator_b_path)
    merged = a[["sample_id", "label"]].merge(
        b[["sample_id", "label"]],
        on="sample_id",
        suffixes=("_a", "_b"),
        how="inner",
    )
    if merged.empty:
        raise ValueError("No overlapping sample_id values found between annotation files")
    return float(cohen_kappa_score(merged["label_a"], merged["label_b"]))
