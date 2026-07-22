from pathlib import Path

base = Path("hlsfactory/hls_dataset_sources/polybench")

designs = ["bicg", "gemm", "gesummv", "k2mm", "k3mm", "mvt", "syr2k", "syrk"]

template = """design_name = "{name}"
dataset_name = "polybench"

[[flow_configs]]
flow_name = "VitisHLSSynthFlow"
synth_tcl = "dataset_hls.tcl"

[[flow_configs]]
flow_name = "VitisHLSCosimSetupFlow"
cosim_setup_tcl = "dataset_hls_cosim_setup.tcl"

[[flow_configs]]
flow_name = "VitisHLSImplFlow"
impl_tcl = "dataset_hls_ip_export.tcl"
"""

for design_name in designs:
    design_dir = base / design_name
    if not design_dir.exists():
        print(f"SKIP (folder not found): {design_name}")
        continue
    toml_path = design_dir / "hlsfactory.toml"
    toml_path.write_text(template.format(name=design_name))
    print(f"Created: {toml_path}")

print("\nDone!")