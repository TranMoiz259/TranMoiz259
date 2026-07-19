## Vietnamese Fake News Detection (Health/Finance Domain)

This repository now implements a reproducible project skeleton for Vietnamese misinformation detection with staged modeling:

1. **Baseline**: TF-IDF + Logistic Regression  
2. **Main model**: PhoBERT fine-tuning  
3. **Enhanced model**: PhoBERT embedding + metadata fusion

It also includes:
- Annotation helpers + Cohen's Kappa computation
- Time-based train/test split
- FastAPI inference API
- Streamlit demo dashboard

## Repository structure

- `/src/vn_fake_news/data_pipeline.py` - loading/cleaning/splitting + annotation QA
- `/src/vn_fake_news/baseline.py` - baseline training + metrics
- `/src/vn_fake_news/phobert.py` - PhoBERT fine-tuning pipeline
- `/src/vn_fake_news/fusion.py` - metadata-fusion modeling
- `/scripts/run_pipeline.py` - end-to-end training runner
- `/api/main.py` - inference API
- `/dashboard/app.py` - demo UI
- `/configs/project.yaml` - project configuration

## Dataset format

Input CSV must include:
- `text`
- `label` (`real`, `fake`, `misleading` or binary labels)
- `published_at` (timestamp)
- `source_type`
- `engagement`
- optional: `domain` (`health`, `finance`, ...)

## Quick start

### 1) Install

```bash
pip install -r requirements.txt
```

### 2) Run training pipeline

```bash
PYTHONPATH=src python scripts/run_pipeline.py \
  --data data/raw/your_dataset.csv \
  --domain health \
  --test-ratio 0.2
```

Outputs are saved in `data/artifacts/` including model artifacts and metrics JSON files.

### 3) Start API

```bash
uvicorn api.main:app --reload
```

- `GET /health`
- `POST /predict` with payload: `{ "text": "..." }`

### 4) Start demo dashboard

```bash
streamlit run dashboard/app.py
```

## Annotation workflow

1. Use two independent annotators with shared labeling guideline.
2. Save each annotator output with `sample_id,label` columns.
3. Compute inter-annotator agreement with `compute_cohen_kappa(...)` in `data_pipeline.py`.

## Evaluation

- Primary: **Macro F1**
- Secondary: per-class precision/recall/F1, confusion matrix
- Split: time-based holdout
- Error analysis: inspect misclassified samples from model outputs
