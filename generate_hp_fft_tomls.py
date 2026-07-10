from pathlib import Path

base = Path("hlsfactory/hls_dataset_sources/hp_fft_hls")

template = """design_name = "{name}"
dataset_name = "hp_fft_hls"

[[flow_configs]]
flow_name = "VitisHLSSynthFlow"
synth_tcl = "dataset_hls.tcl"

[[flow_configs]]
flow_name = "VitisHLSImplFlow"
impl_tcl = "dataset_hls_ip_export.tcl"
"""

created = []

for design_dir in sorted(base.iterdir()):
    if not design_dir.is_dir():
        continue
    design_name = design_dir.name
    toml_path = design_dir / "hlsfactory.toml"
    if toml_path.exists():
        print(f"SKIP (already exists): {design_name}")
        continue
    if not (design_dir / "dataset_hls.tcl").exists():
        print(f"WARNING (no dataset_hls.tcl): {design_name}")
        continue
    toml_path.write_text(template.format(name=design_name))
    print(f"Created: {toml_path}")
    created.append(design_name)

print(f"\nDone! Created {len(created)} files.")