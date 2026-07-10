from pathlib import Path

base = Path("hlsfactory/hls_dataset_sources/machsuite")

designs = [
    "aes_table", "aes_tableless", "backprop", "bfs_bulk", "bfs_queue",
    "fft_transpose", "gemm_blocked", "gemm_ncubed", "md_grid", "md_knn",
    "nw", "sort_merge", "sort_radix", "spmv_crs", "spmv_ellpack",
    "stencil2D", "stencil3D", "viterbi",
]
# note: "common" is excluded - it's shared utility code, not a standalone design

template = """design_name = "{name}"
dataset_name = "machsuite"

[[flow_configs]]
flow_name = "VitisHLSSynthFlow"
synth_tcl = "dataset_hls.tcl"

[[flow_configs]]
flow_name = "VitisHLSImplFlow"
impl_tcl = "dataset_hls_ip_export.tcl"
"""

created = []
skipped = []

for design_name in designs:
    design_dir = base / design_name
    if not design_dir.exists():
        print(f"SKIP (folder not found): {design_name}")
        skipped.append(design_name)
        continue
    toml_path = design_dir / "hlsfactory.toml"
    toml_path.write_text(template.format(name=design_name))
    print(f"Created: {toml_path}")
    created.append(design_name)

print(f"\nDone! Created {len(created)} files, skipped {len(skipped)}.")