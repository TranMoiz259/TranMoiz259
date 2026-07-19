from __future__ import annotations

from pathlib import Path

import joblib
import streamlit as st

st.set_page_config(page_title="VN Fake News Demo", layout="centered")
st.title("Vietnamese Fake News Risk Scoring Demo")

model_path = Path("data/artifacts/baseline_tfidf_lr.joblib")
if not model_path.exists():
    st.warning("Model not found. Train baseline model first at data/artifacts/baseline_tfidf_lr.joblib")
    st.stop()

model = joblib.load(model_path)
text = st.text_area("Input Vietnamese news/social content")

if st.button("Predict"):
    if not text.strip():
        st.error("Please input text.")
    else:
        label = model.predict([text])[0]
        st.success(f"Prediction: {label}")
