# Altera HLS IP Gen reference designs

These self-contained designs come from
[Altera hls-samples/ReferenceDesigns](https://github.com/altera-fpga/hls-samples/tree/0abae6d78af5daca3fe5d67e617ab037e58aff89/ReferenceDesigns),
commit `0abae6d78af5daca3fe5d67e617ab037e58aff89`.
Each directory preserves Altera's MIT-style license in `License.txt`.
The C++ sources and shared headers are vendored unchanged from that revision;
HLSFactory adds TOML configuration in place of the upstream CMake build.
The dataset contains one synthesis configuration for each of the 17 upstream
reference-design folders. Newly imported designs use upstream's default
Agilex7 report-target settings, extracted by configuring their CMake projects,
except Gzip uses the documented single-engine setting to limit synthesis cost.

| Design | Configured variant |
| --- | --- |
| `anr` | Adaptive noise reduction, one pixel per cycle |
| `cholesky` | Real 32×32 Cholesky decomposition |
| `cholesky_inversion` | Real 32×32 Cholesky-based matrix inversion |
| `convolution2d` | 3×3 convolution, two 10-bit pixels per cycle |
| `crr` | Cox–Ross–Rubinstein option pricing, inner unroll 64 |
| `db` | Database query 1; sources for other upstream queries are also retained |
| `decompress` | Snappy decompression; upstream Gzip headers also retained |
| `fft2d` | 1024×1024 FFT, parallelism 8 |
| `gzip` | High-bandwidth compression, one engine |
| `matmul` | 64×64 matrix multiplication with 8×8 tiles |
| `merge_sort` | Two merge units, sort width two |
| `mvdr_beamforming` | Default sensor array, USM and emulated I/O pipes |
| `niosv` | The reference system's `simple_dma` HLS accelerator |
| `pca` | Benchmark-mode principal component analysis |
| `qrd` | Complex 128×128 QR decomposition |
| `qri` | Real 32×32 QR-based matrix inversion |
| `svd` | Default singular value decomposition |

For the 15 newly imported designs, `README.upstream.md` records the upstream
instructions and `third-party-programs.txt` preserves the repository's additional
license notices. Runtime datasets, screenshots and board-system projects are
not needed for HLS synthesis and are not included. In particular, the `niosv`
entry synthesizes the DMA IP, not the surrounding Nios V processor system.
Each design is self-contained when copied independently: shared headers are
inside its own `include/`; convolution-specific headers are in `design_include/`.

* `matmul`: streaming systolic floating-point matrix multiplication, using
  the upstream 64×64 matrices and 8×8 tiles. Emulation checks two matrix
  products against the software reference with one repetition.
* `merge_sort`: pipelined merge sorting with two merge units and sort width
  two. Emulation checks 128 integers with two runs and seed 777.

Load with `datasets_builder(work_dir, ["altera_examples"])`.
All 17 designs have `AlteraHLSSynthFlow` and `AlteraQuartusImplFlow` entries.
Only `matmul` and `merge_sort` additionally have validated `AlteraHLSCsimFlow`
and `AlteraHLSCosimFlow` entries. No OptDSL frontend is needed.
See [the flow documentation](../../../docs/source/tutorials/altera_flow.md) for setup,
configuration and validation commands.

Co-simulation uses the upstream `FPGA_SIMULATOR` host-code branches:
one 64×64 matrix product for `matmul`, and 16 integers with two runs for
`merge_sort`. Merge sort additionally captures top-level Questa waveforms.

Validate synthesis of the entire dataset from the repository root:

```bash
uv run python tests/dataset_validator.py \
  hlsfactory/hls_dataset_sources/altera_examples \
  --flow AlteraHLSSynthFlow --timeout 3600 -j 4 \
  --work-dir /tmp/hlsfactory-altera-all17-synth
```

Synthesis validation generates RTL and estimated resources; it does not run
Quartus place-and-route or establish timing closure. Use
`--designs matmul merge_sort` when selecting an emulation or co-simulation flow.

All 17 configurations passed `AlteraHLSSynthFlow` validation with Altera HLS IP
Gen 2026.1.0 on September 7, 2026. The single-engine Gzip configuration took
approximately 600 seconds; the other designs completed within 134 seconds each.
