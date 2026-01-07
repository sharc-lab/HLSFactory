"""Tests for Design and DesignDataset config integration."""

from __future__ import annotations

from pathlib import Path

import pytest

from hlsfactory.design_config import (
    DesignConfig,
    DesignConfigError,
    FlowConfig,
    write_design_config,
)
from hlsfactory.framework import Design, DesignDataset


def _create_design_with_config(
    design_dir: Path,
    design_name: str,
    dataset_name: str,
    flow_configs: list[FlowConfig] | None = None,
) -> None:
    """Helper to create a design directory with hlsfactory.toml."""
    design_dir.mkdir(parents=True, exist_ok=True)

    config = DesignConfig(
        design_name=design_name,
        dataset_name=dataset_name,
        tags=[],
        env_vars={},
        flow_configs=flow_configs or [],
    )
    write_design_config(design_dir / "hlsfactory.toml", config)


class TestDesignWithConfig:
    """Tests for Design class config handling."""

    def test_design_without_config(self, tmp_path: Path) -> None:
        """Test that Design without config has config=None."""
        design = Design("test", tmp_path)
        assert design.config is None

    def test_design_require_config_raises_without_config(self, tmp_path: Path) -> None:
        """Test that require_config raises when no config is loaded."""
        design = Design("test", tmp_path)
        with pytest.raises(DesignConfigError):
            design.require_config()

    def test_design_with_config(self, tmp_path: Path) -> None:
        """Test that Design can hold a config."""
        config = DesignConfig(
            design_name="test",
            dataset_name="test_dataset",
            flow_configs=[],
        )
        design = Design("test", tmp_path, config=config)

        assert design.config is not None
        assert design.config.design_name == "test"
        assert design.require_config() is config

    def test_design_from_dir_with_config(self, tmp_path: Path) -> None:
        """Test Design.from_dir_with_config factory method."""
        design_dir = tmp_path / "test_design"
        _create_design_with_config(
            design_dir,
            design_name="test_design",
            dataset_name="test_dataset",
            flow_configs=[
                FlowConfig(
                    flow_name="VitisHLSSynthFlow",
                    flow_settings={"synth_tcl": "run.tcl"},
                )
            ],
        )

        design = Design.from_dir_with_config(design_dir)

        assert design.name == "test_design"
        assert design.config is not None
        assert design.config.design_name == "test_design"
        assert design.config.supports_flow("VitisHLSSynthFlow")

    def test_design_from_dir_with_config_missing_file(self, tmp_path: Path) -> None:
        """Test that from_dir_with_config raises when config file is missing."""
        design_dir = tmp_path / "test_design"
        design_dir.mkdir()

        with pytest.raises(DesignConfigError):
            Design.from_dir_with_config(design_dir)

    def test_copy_and_rename_preserves_config(self, tmp_path: Path) -> None:
        """Test that copying a design preserves or reloads config."""
        design_dir = tmp_path / "original"
        _create_design_with_config(
            design_dir,
            design_name="original",
            dataset_name="test_dataset",
        )

        design = Design.from_dir_with_config(design_dir)
        new_design = design.copy_and_rename_to_new_parent_dir("copy", tmp_path)

        assert new_design.config is not None
        # The config is preserved from original since config file exists in copy
        assert new_design.config.design_name == "original"


class TestDesignDatasetWithConfig:
    """Tests for DesignDataset config loading."""

    def test_from_dir_requires_config_by_default(self, tmp_path: Path) -> None:
        """Test that from_dir raises when config is missing (require_config=True)."""
        dataset_dir = tmp_path / "dataset"
        dataset_dir.mkdir()

        design_dir = dataset_dir / "design1"
        design_dir.mkdir()
        # No hlsfactory.toml created

        with pytest.raises(DesignConfigError):
            DesignDataset.from_dir("test", dataset_dir)

    def test_from_dir_with_require_config_false(self, tmp_path: Path) -> None:
        """Test from_dir with require_config=False allows missing configs."""
        dataset_dir = tmp_path / "dataset"
        dataset_dir.mkdir()

        design_dir = dataset_dir / "design1"
        design_dir.mkdir()
        # No hlsfactory.toml created

        dataset = DesignDataset.from_dir("test", dataset_dir, require_config=False)
        assert len(dataset.designs) == 1
        assert dataset.designs[0].config is None

    def test_from_dir_loads_configs(self, tmp_path: Path) -> None:
        """Test that from_dir loads configs for all designs."""
        dataset_dir = tmp_path / "dataset"
        dataset_dir.mkdir()

        # Create two designs with configs
        for name in ["design1", "design2"]:
            _create_design_with_config(
                dataset_dir / name,
                design_name=name,
                dataset_name="test_dataset",
                flow_configs=[
                    FlowConfig(
                        flow_name="VitisHLSSynthFlow",
                        flow_settings={"synth_tcl": "run.tcl"},
                    )
                ],
            )

        dataset = DesignDataset.from_dir("test", dataset_dir)

        assert len(dataset.designs) == 2
        for design in dataset.designs:
            assert design.config is not None
            assert design.config.supports_flow("VitisHLSSynthFlow")

    def test_from_dir_with_exclude_filter(self, tmp_path: Path) -> None:
        """Test that exclude_dir_filter still works with config loading."""
        dataset_dir = tmp_path / "dataset"
        dataset_dir.mkdir()

        # Create designs
        for name in ["design1", "excluded_design"]:
            _create_design_with_config(
                dataset_dir / name,
                design_name=name,
                dataset_name="test_dataset",
            )

        dataset = DesignDataset.from_dir(
            "test",
            dataset_dir,
            exclude_dir_filter=lambda p: p.name.startswith("excluded"),
        )

        assert len(dataset.designs) == 1
        assert dataset.designs[0].name == "design1"

    def test_from_dir_sorts_designs_by_name(self, tmp_path: Path) -> None:
        """Test that designs are sorted by name."""
        dataset_dir = tmp_path / "dataset"
        dataset_dir.mkdir()

        # Create designs in non-alphabetical order
        for name in ["charlie", "alice", "bob"]:
            _create_design_with_config(
                dataset_dir / name,
                design_name=name,
                dataset_name="test_dataset",
            )

        dataset = DesignDataset.from_dir("test", dataset_dir)

        names = [d.name for d in dataset.designs]
        assert names == ["alice", "bob", "charlie"]
