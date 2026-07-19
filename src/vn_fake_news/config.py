from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class ProjectPaths:
    root: Path

    @property
    def raw_data(self) -> Path:
        return self.root / "data" / "raw"

    @property
    def processed_data(self) -> Path:
        return self.root / "data" / "processed"

    @property
    def annotations(self) -> Path:
        return self.root / "data" / "annotations"

    @property
    def artifacts(self) -> Path:
        return self.root / "data" / "artifacts"

    @property
    def reports(self) -> Path:
        return self.root / "data" / "reports"
