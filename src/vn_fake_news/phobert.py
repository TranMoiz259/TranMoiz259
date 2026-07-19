from __future__ import annotations

import json
from pathlib import Path
from typing import Dict

import pandas as pd
from datasets import Dataset
from sklearn.metrics import f1_score, precision_recall_fscore_support
from transformers import (
    AutoModelForSequenceClassification,
    AutoTokenizer,
    DataCollatorWithPadding,
    Trainer,
    TrainingArguments,
)

MODEL_NAME = "vinai/phobert-base"


def _build_label_maps(train_df: pd.DataFrame) -> tuple[dict[str, int], dict[int, str]]:
    labels = sorted(train_df["label"].unique())
    label2id = {label: idx for idx, label in enumerate(labels)}
    id2label = {idx: label for label, idx in label2id.items()}
    return label2id, id2label


def train_phobert_classifier(
    train_df: pd.DataFrame,
    test_df: pd.DataFrame,
    artifact_dir: str | Path,
    epochs: int = 2,
    batch_size: int = 8,
) -> Dict[str, float]:
    label2id, id2label = _build_label_maps(train_df)
    tokenizer = AutoTokenizer.from_pretrained(MODEL_NAME)

    def tokenize(batch):
        return tokenizer(batch["text"], truncation=True, max_length=256)

    train_ds = Dataset.from_pandas(train_df[["text", "label"]].copy())
    test_ds = Dataset.from_pandas(test_df[["text", "label"]].copy())
    train_ds = train_ds.map(lambda x: {"labels": label2id[x["label"]]})
    test_ds = test_ds.map(lambda x: {"labels": label2id[x["label"]]})
    train_ds = train_ds.map(tokenize, batched=True)
    test_ds = test_ds.map(tokenize, batched=True)

    model = AutoModelForSequenceClassification.from_pretrained(
        MODEL_NAME,
        num_labels=len(label2id),
        label2id=label2id,
        id2label=id2label,
    )

    args = TrainingArguments(
        output_dir=str(Path(artifact_dir) / "phobert"),
        num_train_epochs=epochs,
        per_device_train_batch_size=batch_size,
        per_device_eval_batch_size=batch_size,
        evaluation_strategy="epoch",
        save_strategy="epoch",
        load_best_model_at_end=True,
        metric_for_best_model="macro_f1",
        report_to=[],
    )

    def compute_metrics(eval_pred):
        logits, labels = eval_pred
        preds = logits.argmax(axis=1)
        p, r, f1, _ = precision_recall_fscore_support(labels, preds, average="macro", zero_division=0)
        return {
            "macro_f1": float(f1_score(labels, preds, average="macro")),
            "macro_precision": float(p),
            "macro_recall": float(r),
            "macro_f1_prfs": float(f1),
        }

    trainer = Trainer(
        model=model,
        args=args,
        train_dataset=train_ds,
        eval_dataset=test_ds,
        tokenizer=tokenizer,
        data_collator=DataCollatorWithPadding(tokenizer=tokenizer),
        compute_metrics=compute_metrics,
    )
    trainer.train()
    metrics = trainer.evaluate()

    out = Path(artifact_dir)
    out.mkdir(parents=True, exist_ok=True)
    trainer.save_model(str(out / "phobert"))
    (out / "phobert_metrics.json").write_text(json.dumps(metrics, ensure_ascii=False, indent=2), encoding="utf-8")
    return {k: float(v) for k, v in metrics.items() if isinstance(v, (int, float))}
