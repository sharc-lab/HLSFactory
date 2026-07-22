import sys
from pathlib import Path

sys.path.insert(0, "hlsfactory")
from hlsfactory.design_config import read_design_config, DesignConfigError

base = Path("hlsfactory/hls_dataset_sources")

print("=" * 70)
print("HLSFactory Design Config Audit")
print("=" * 70)

for dataset_dir in sorted(base.iterdir()):
    if not dataset_dir.is_dir():
        continue
    dataset_name = dataset_dir.name

    designs = [d for d in dataset_dir.iterdir() if d.is_dir()]

    with_toml = []
    without_toml = []
    errors = []

    for design in sorted(designs):
        toml_path = design / "hlsfactory.toml"
        if toml_path.exists():
            with_toml.append(design.name)
            try:
                config = read_design_config(toml_path)
            except DesignConfigError as e:
                errors.append((design.name, str(e)))
            except Exception as e:
                errors.append((design.name, f"Unexpected error: {e}"))
        else:
            without_toml.append(design.name)

    if with_toml or without_toml:
        print(f"\n{dataset_name}")
        print(f"  Designs WITH hlsfactory.toml:    {len(with_toml)}")
        print(f"  Designs WITHOUT hlsfactory.toml: {len(without_toml)}")
        if errors:
            print(f"  VALIDATION ERRORS: {len(errors)}")
            for name, err in errors:
                print(f"    - {name}: {err}")
        if without_toml and len(without_toml) <= 25:
            print(f"  Missing in: {', '.join(without_toml)}")

print("\n" + "=" * 70)
print("Done.")
print("=" * 70)