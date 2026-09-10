# Frontend demos

Both demos implement `out[i] = scale * a[i] + b[i]` and generate exactly four
design variants: `(vector_size, scale)` = `(16, 2)`, `(16, 4)`, `(32, 2)`, `(32, 4)`.

- `demo_frontend_jinja.py` passes four dictionaries to `JinjaFrontend`, which
  renders `vector_affine.cpp.jinja` into C++ source. It also sets `lanes` to
  `1`, `2`, `1`, and `4`, respectively, to demonstrate structural parameterization:
  Jinja `{% if lanes > 1 %}` / `{% else %}` selects either a parallel implementation
  with partitioned arrays or a scalar loop. Jinja `{% for %}` loops emit partition
  pragmas for each array and an independent assignment for each parallel lane.
  These branches and loops execute during template rendering; Vitis receives only
  the selected C++ implementation. Keep `vector_size` divisible by `lanes`.
- `demo_frontend_define.py` passes four dictionaries of macro definitions to
  `CPPPreprocessorFrontend`, which runs `g++ -E` to produce concrete C++ source.

Each script then passes all four generated designs to `VitisHLSSynthFlow` for
`csynth_design`, targeting `xc7z020clg400-1` with a 10 ns clock. Edit the design's
`dataset_hls.tcl` to change the part or clock.

From the repository root, with Vitis HLS configured and `vitis_hls` on `PATH`
(and `g++` on `PATH` for the define demo), run:

```bash
uv run python demos/demo_frontends/demo_frontend_jinja.py
uv run python demos/demo_frontends/demo_frontend_define.py
```

Optional arguments are `--work-dir PATH`, `--vitis-hls-bin PATH`, and
`--timeout SECONDS` (300 seconds per variant by default). Each run uses a new
temporary directory unless `--work-dir` is supplied; use a fresh output directory
for each run. The scripts print the output directory and each configuration's
generated design path. Each successful variant contains the Vitis synthesis
reports, `data_hls.json`, `data_design.json`, and execution metadata. A synthesis
failure makes the demo exit with an error after attempting the remaining variants.
