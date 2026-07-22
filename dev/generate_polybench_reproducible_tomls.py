from pathlib import Path

base = Path("hlsfactory/hls_dataset_sources/polybench__reproducible")

template = """design_name = "{group}__{name}"
dataset_name = "polybench__reproducible"

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

created = []

for group_dir in sorted(base.iterdir()):
    if not group_dir.is_dir():
        continue
    group_name = group_dir.name
    for design_dir in sorted(group_dir.iterdir()):
        if not design_dir.is_dir():
            continue
        design_name = design_dir.name
        toml_path = design_dir / "hlsfactory.toml"
        if toml_path.exists():
            print(f"SKIP (already exists): {group_name}/{design_name}")
            continue
        if not (design_dir / "dataset_hls.tcl").exists():
            print(f"WARNING (no dataset_hls.tcl): {group_name}/{design_name}")
            continue
        toml_path.write_text(template.format(group=group_name, name=design_name))
        print(f"Created: {toml_path}")
        created.append(design_name)

print(f"\nDone! Created {len(created)} files.")