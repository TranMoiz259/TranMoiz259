from __future__ import annotations

import argparse
import json
from pathlib import Path

from vn_fake_news.baseline import train_and_evaluate_baseline
from vn_fake_news.data_pipeline import filter_domain, load_dataset, time_based_split
from vn_fake_news.fusion import train_fusion_model
from vn_fake_news.phobert import train_phobert_classifier


def main() -> None:
    parser = argparse.ArgumentParser(description="Run Vietnamese fake news detection pipeline")
    parser.add_argument("--data", required=True, help="Path to source CSV")
    parser.add_argument("--domain", default="health", help="Domain filter (health|finance)")
    parser.add_argument("--test-ratio", type=float, default=0.2)
    parser.add_argument("--artifact-dir", default="data/artifacts")
    parser.add_argument("--skip-phobert", action="store_true")
    parser.add_argument("--skip-fusion", action="store_true")
    args = parser.parse_args()

    df = load_dataset(args.data)
    df = filter_domain(df, args.domain)
    train_df, test_df = time_based_split(df, test_ratio=args.test_ratio)

    artifact_dir = Path(args.artifact_dir)
    artifact_dir.mkdir(parents=True, exist_ok=True)

    results = {
        "baseline": train_and_evaluate_baseline(train_df, test_df, artifact_dir),
    }

    if not args.skip_phobert:
        results["phobert"] = train_phobert_classifier(train_df, test_df, artifact_dir)

    if not args.skip_fusion:
        results["fusion"] = train_fusion_model(train_df, test_df, artifact_dir)

    (artifact_dir / "summary_metrics.json").write_text(
        json.dumps(results, ensure_ascii=False, indent=2),
        encoding="utf-8",
    )
    print(json.dumps(results, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
