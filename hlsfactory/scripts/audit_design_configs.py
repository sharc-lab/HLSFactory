"""Audit hlsfactory.toml coverage and validity across all design datasets.

Scans every dataset directory under hls_dataset_sources/ and reports, per
dataset, how many designs have an hlsfactory.toml, how many are missing one,
and whether any existing config files fail to parse/validate against the
DesignConfig schema.

Usage:
    uv run python -m hlsfactory.scripts.audit_design_configs
"""

from __future__ import annotations

from pathlib import Path

from hlsfactory.design_config import DesignConfigError, read_design_config


def main() -> int:
    script_dir = Path(__file__).resolve().parent
    base = script_dir.parent / "hls_dataset_sources"

    if not base.exists():
        print(f"ERROR: Dataset sources not found at {base}")
        return 1

    print("=" * 70)
    print("HLSFactory Design Config Audit")
    print("=" * 70)

    total_errors = 0

    for dataset_dir in sorted(base.iterdir()):
        if not dataset_dir.is_dir():
            continue
        dataset_name = dataset_dir.name

        designs = [d for d in dataset_dir.iterdir() if d.is_dir()]

        with_toml: list[str] = []
        without_toml: list[str] = []
        errors: list[tuple[str, str]] = []

        for design in sorted(designs):
            toml_path = design / "hlsfactory.toml"
            if toml_path.exists():
                with_toml.append(design.name)
                try:
                    read_design_config(toml_path)
                except DesignConfigError as e:
                    errors.append((design.name, str(e)))
                except Exception as e:  # noqa: BLE001 - report any parse failure
                    errors.append((design.name, f"Unexpected error: {e}"))
            else:
                without_toml.append(design.name)

        if with_toml or without_toml:
            print(f"\n{dataset_name}")
            print(f"  Designs WITH hlsfactory.toml:    {len(with_toml)}")
            print(f"  Designs WITHOUT hlsfactory.toml: {len(without_toml)}")
            if errors:
                total_errors += len(errors)
                print(f"  VALIDATION ERRORS: {len(errors)}")
                for name, err in errors:
                    print(f"    - {name}: {err}")
            if without_toml and len(without_toml) <= 25:
                print(f"  Missing in: {', '.join(without_toml)}")

    print("\n" + "=" * 70)
    print(f"Done. {total_errors} validation error(s) found.")
    print("=" * 70)

    return 1 if total_errors else 0


if __name__ == "__main__":
    raise SystemExit(main())