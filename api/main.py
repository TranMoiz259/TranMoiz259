from __future__ import annotations

from pathlib import Path

import joblib
from fastapi import FastAPI, HTTPException
from pydantic import BaseModel

app = FastAPI(title="Vietnamese Fake News Risk Scoring API", version="0.1.0")

MODEL_PATH = Path("data/artifacts/baseline_tfidf_lr.joblib")
_model = None


class PredictRequest(BaseModel):
    text: str


class PredictResponse(BaseModel):
    label: str


@app.on_event("startup")
def load_model() -> None:
    global _model
    if MODEL_PATH.exists():
        _model = joblib.load(MODEL_PATH)


@app.get("/health")
def health() -> dict:
    return {"status": "ok", "model_loaded": _model is not None}


@app.post("/predict", response_model=PredictResponse)
def predict(payload: PredictRequest) -> PredictResponse:
    if _model is None:
        raise HTTPException(status_code=503, detail="Model is not loaded. Train baseline model first.")
    pred = _model.predict([payload.text])[0]
    return PredictResponse(label=str(pred))
