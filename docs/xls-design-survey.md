# Public XLS / DSLX designs for an HLS dataset

Survey date: September 9, 2026.

This is a broad discovery inventory of public hardware designs targeting Google's XLS, primarily DSLX, with separate notes for XLS[cc] and generators. It is not a claim to have found every design on the internet. Repository directories, selected source files, project documentation, and publications were inspected; designs were not compiled, simulated, or synthesized during this survey.

The strongest starting points are Zstandard, AES modes, the standalone JPEG encoder, the XLS32 audio engine, MachSuite FFT, and the floating-point ports. hls4ml offers an additional route to larger generated designs. A new ECO paper provides particularly relevant leads for Histogram and Vector Core.

Priority reflects likely dataset value, not measured hardware size:
- **P1:** substantial application, algorithm, or reusable computation.
- **P2:** useful smaller kernel, derivative, or integration example.
- **Lead:** a publication or generator needs further artifact extraction or validation.
“Source verified” means files or source directories were observed, not that the design passes tests.

## 1. Substantial source collections and independent projects

| Priority | Design / collection | Direct source | What is available and why it matters | Qualification |
|---|---|---|---|---|
| P1 | Zstandard decoder | [google/xls: zstd](https://github.com/google/xls/tree/main/xls/modules/zstd) | Integrated decoder plus compressed-block, literals, FSE, Huffman, sequence, buffer and memory components. Top-level source: zstd_dec.x. | Source verified. Treat the full decoder and extracted blocks as one related family. Apache-2.0 project; external RTL dependencies also matter. |
| P1 | AES, AES-CTR, AES-GCM, GHASH | [google/xls: aes](https://github.com/google/xls/tree/main/xls/modules/aes) | aes.x, aes_ctr.x, aes_gcm.x, ghash.x, shared code and C++ tests. Provides block computation and stateful modes. | Source verified. Shared AES internals create strong overlap between entries. Apache-2.0. |
| P1 | Hardware JPEG encoder | [HW_JPEGenc/src](https://github.com/nishiharu1977/HW_JPEGenc/tree/main/src) | DCT_1D, DCT_2D, DCT_2D_proc, quantization, zigzag, RGB–YCbCr, Huffman AC/DC encoding, data rearrangement. | Source verified. Independent project, experimental. README states MIT; a root LICENSE was not visible in the inspected listing. The 8×8 image buffer is outside DSLX. |
| P1 | XLS32 polyphonic audio synthesizer | [core/synth.x](https://github.com/kazunori279/xls32-fpga-synth/blob/main/core/synth.x) | Stateful voice engine combining oscillators, envelopes, filtering, voice allocation, MIDI handling and mixing. | Source file verified, architecture documented. Apache-2.0. Project explicitly identifies AI-agent development. Effects and board shells are outside DSLX; count the DSLX engine separately from the whole FPGA system. |
| P1 | RISC-V cryptographic accelerator | [MASC masc.x](https://github.com/masc-ucsc/MASC-AI-Synthesized-Cryptoprocessor/blob/main/AI_generated/top_level/masc.x) | RV32 crypto operation accelerator with bit manipulation, AES decryption and SHA operations; accompanying IR and RTL. | Source verified. Apache-2.0 badge in README. Explicitly GPT-4-generated, with reported Spike cosimulation. It is an accelerator using its own opcodes, not a complete CPU. |
| P1 | Weight-stationary FP32 systolic matmul | [ws_matmul.x](https://github.com/google/xls/blob/main/xls/modules/ml/ws_matmul.x) | Parameterized array of floating-point processing elements and communicating processes. | Source inspected; Apache-2.0. Distinct from the older 4×4 example, but related implementations must be checked for overlap. |
| P1 / generator | hls4ml XLS backend | [DSLX layer library](https://github.com/fastmachinelearning/hls4ml/tree/main/hls4ml/templates/xls/firmware/nnet_utils) | dense.x, conv1d.x, conv2d.x, depthwise_conv.x, pooling.x, batchnorm.x, activations.x, merge.x and shape/data-format helpers. | Source directory verified. These are reusable modules/templates, not a catalog of already generated trained networks. Preserve model architecture, weights, precision, generator revision and generated DSLX. |
| P1 / lead | Physics jet-tagging neural network | [XLS discussion #2919, code attachment](https://github.com/google/xls/discussions/2919) | Four dense layers with constant weights and ReLU/Argmax; three proc partitionings; generation and Verilator scripts described. | Public attachment linked; archive contents and license not independently inspected. Author reports throughput problems and questionable behavior for one variant. Preserve as a candidate, not a validated benchmark. |
| P1 / generator | Allo-to-XLS GEMM and systolic arrays | [Nikil-Shyamsunder backend examples](https://github.com/Nikil-Shyamsunder/allo-xls-backend/tree/main/allo/backend/xls/examples) | GEMM transformation families and generated integer/floating-point systolic arrays, with test and generation workflows. | Backend README inspected. Course report describes 2×2–4×4 systolic validation and larger function GEMM cases. Generator variants are not independent algorithms. License needs file-level confirmation. |
| P1 / lead | Histogram and Vector Core | [xls-eco paper branch](https://github.com/alirezazd/xls-eco/tree/eco-paper-2025) | Two custom designs introduced by the ECO paper, in addition to five reused/adapted XLS designs. | Branch and ECO implementation verified, but exact custom benchmark files were not located in inspected directories. Do not mark these ready for ingestion until files are recovered. |
| P2 | SECDED error-correction encoder/decoder | [bedrock-dslx/lib/ecc](https://github.com/xlsynth/bedrock-dslx/tree/main/lib/ecc) | Parameterized encoder, decoder and tests. | Source inspected. Apache-2.0. Good coding-theory coverage; widths are configurations of the same family. |
| P2 | Convolution engine / ring buffer | [hzeller/xls-convolve](https://github.com/hzeller/xls-convolve) | convolve.x and ringbuffer.x with build/test recipes. | Source files verified. README describes an experiment with time-multiplexing intent. No repository license identified. |
| P2 | Video timing and TMDS encoding | [xls-tang-nano-lcd](https://github.com/jumang4423/xls-tang-nano-lcd) | Documented dslx/video.x handles timing, animation state, pixel composition and TMDS encoding; surrounding RTL handles storage, clocks and PHY. | README inspected. Reported simulation workflow; README says display on physical hardware was not checked. No repository license identified. |
| P2 | Polynomial sampler with SPI and step/direction output | [lromor/tinytapeout](https://github.com/lromor/tinytapeout) | top.x, spi.x and iterative_polynomial_sampler.x described with test/build flow and FPGA harness. | README inspected. Apache-2.0. More useful control/dataflow coverage than a bare arithmetic example. |
| P2 | HSV-to-RGB Caravel accelerator | [proppy/caravel_hsv_mixer](https://github.com/proppy/caravel_hsv_mixer) | Color conversion port and ASIC integration. | README inspected. Apache-2.0 badge, with upstream conversion code requiring its own provenance. Check duplication with XLS third_party/xls_colors. |
| P2 / derivative | OpenMPW RLE and 8-bit FPU | [proppy/xls-openmpw-tapeouts](https://github.com/proppy/xls-openmpw-tapeouts) | RLE encoder/decoder and 8-bit FPU integration, with physical-design artifacts. | Apache-2.0. Explicitly built from Google XLS examples/stdlib; valuable implementation evidence, not new algorithm families. |
| P2 | FP16 multiplier from ManabiSystolicArray | [Verilog/fp16_mul](https://github.com/nishiharu1977/ManabiSystolicArray/tree/main/Verilog/fp16_mul) | README identifies fp16_mul.x and generated pipeline variants. | Only the FP16 multiplier is identified as DSLX. The systolic array and RISC-V cores are Verilog; the multiplier is not yet integrated into the main datapath. |

Zstandard has unusually useful decomposition opportunities: FSE probability/frequency decoding and table creation, Huffman weight/code/literal decoding, sequence decoding/execution, and AXI memory access. Keep the surrounding channel and memory contracts. Its documentation notes a Verilog wrapper and external AXI crossbar, and limits the described integration generator to valid frames. These are material reproduction details. [Design documentation](https://github.com/google/xls/blob/main/xls/modules/zstd/README.md), [Antmicro technical article, April 2025](https://antmicro.com/blog/2025/04/building-a-zstd-decoder-in-xls).

## 2. Upstream kernels worth collecting individually

| Priority | Family | Direct source | Dataset note |
|---|---|---|---|
| P1 | MachSuite FFT port | [fft.x](https://github.com/google/xls/blob/main/third_party/xls_machsuite/fft/fft.x) | Actual DSLX FFT port with reference/test data; BSD-3 license in source. Do not infer that the entire MachSuite has been ported. |
| P1 | Floating-point division | [fpdiv_2x32.x](https://github.com/google/xls/blob/main/third_party/xls_berkeley_softfloat/fpdiv_2x32.x) | SoftFloat-derived algorithm; retain its separate license. |
| P1 | Floating-point exponential, square root, sine/cosine and argument reduction | [xls_go_math](https://github.com/google/xls/tree/main/third_party/xls_go_math) | fpexp_32.x, fpsqrt_32.x, fp_sincos_32.x, fp_trig_reduce.x; C++ tests and separate LICENSE. |
| P1 | SHA-256 | [sha256.x](https://github.com/google/xls/blob/main/xls/examples/sha256.x) | Strong arithmetic/bitwise workload, also used in published XLS scheduling evaluation. |
| P1 | Chen IDCT | [idct_chen.x](https://github.com/google/xls/blob/main/xls/examples/jpeg/idct_chen.x) | Image transform kernel with reference/test infrastructure. |
| P1 | Floating-point arithmetic and FMA | [apfloat.x](https://github.com/google/xls/blob/main/xls/dslx/stdlib/apfloat.x), [apfloat_fmac.x](https://github.com/google/xls/blob/main/xls/examples/apfloat_fmac.x) | Use substantive arithmetic tops; do not count thin wrappers and predicates as separate substantial designs. |
| P1 | Fast reciprocal square root | [float32.x](https://github.com/google/xls/blob/main/xls/dslx/stdlib/float32.x) | fast_rsqrt and configurable refinement entry points. |
| P1/P2 | FIR filter | [fir_filter.x](https://github.com/google/xls/blob/main/xls/examples/fir_filter.x) | Filtering kernel; preserve coefficient and width configuration. |
| P1/P2 | Sobel image filter | [sobel_filter.x](https://github.com/google/xls/blob/main/xls/examples/sobel_filter.x) | Image-processing coverage; also has a benchmark wrapper. |
| P1/P2 | Systolic 4×4 matmul | [matmul_4x4](https://github.com/google/xls/tree/main/xls/examples/matmul_4x4) | Proc-network example; pin tool revision before accepting codegen behavior. |
| P2 | RLE encoder and decoder | [modules/rle](https://github.com/google/xls/tree/main/xls/modules/rle) | Stateful stream processing with tests; less complex than Zstandard. |
| P2 | Bitonic sorting | [bitonic_sort.x](https://github.com/google/xls/blob/main/xls/examples/bitonic_sort.x) | Useful at meaningful array sizes. |
| P2 | Reorder queue | [reorder_queue.x](https://github.com/google/xls/blob/main/xls/examples/reorder_queue/reorder_queue.x) | Useful control/state coverage. |
| P2 | Cubic Bézier | [cubic_bezier.x](https://github.com/google/xls/blob/main/xls/examples/cubic_bezier.x) | Geometry kernel. |
| P2 | CRC32 and Adler32 | [crc32](https://github.com/google/xls/tree/main/xls/examples/crc32), [adler32](https://github.com/google/xls/tree/main/xls/examples/adler32) | Smaller checksum families; retain as secondary coverage. |
| P2 | Hack CPU | [hack_cpu.x](https://github.com/google/xls/blob/main/xls/examples/hack_cpu.x) | Educational processor family, below a production CPU in complexity. |
| Low | Simple RISC-V | [riscv_simple.x](https://github.com/google/xls/blob/main/xls/examples/riscv_simple.x) | Source explicitly says only a very small ISA subset is implemented. Do not describe it as a complete RV32I core. |

Additional secondary candidates visible in the [examples directory](https://github.com/google/xls/tree/main/xls/examples) include prefix_sum, sparse_prefix_sum, dot_product, memory_proc, ram and serialized_decomposer. Exclude language demonstrations, tiny adders, passthrough designs and compiler regression tests from the core nontrivial corpus.

## 3. Publications and artifact connections

| Publication | Designs or evidence | Source status and use |
|---|---|---|
| [Subgraph Extraction-based Feedback-guided Iterative Scheduling for HLS](https://hanchenye.com/assets/pdfs/DATE24_ISDC.pdf), DATE 2024, Ye et al. | 17 benchmark rows, including SHA-256, exponential, reciprocal square root, CRC32, HSV conversion, binary division, and industrial ML/video/internal datapaths. | Several public kernels map to upstream XLS. The industrial datapath sources were not located. Open-source scheduler code does not establish that every benchmark is public. |
| [High-Level Synthesis versus Hardware Construction](https://research.tudelft.nl/en/publications/high-level-synthesis-versus-hardware-construction/), Kamkin et al., 2023 | 8×8 IDCT comparison across DSLX and other languages; the paper describes 19 XLS configurations. | [ispras/hls-idct/src/dslx](https://github.com/ispras/hls-idct/tree/main/src/dslx). README attributes the DSLX implementation to Google XLS. Count as an adapted IDCT family, not 19 unrelated designs. |
| [Graph-Based ECO and Patch Generation for High-Level Synthesis](https://arxiv.org/html/2604.14248v1), Azadi et al., 2026 preprint | Seven designs. Histogram and Vector Core were created for the evaluation; the other five are taken/adapted from XLS. Reported IR node counts: Histogram 807; Vector Core 988. | [Published branch](https://github.com/alirezazd/xls-eco/tree/eco-paper-2025) exists; exact custom files still need locating. The paper reports incomplete Histogram equivalence verification. |
| [Correct and Compositional Hardware Generators](https://arxiv.org/html/2401.02570v1), 2024 preprint version | Parafil iterative FFT using XLS-generated floating-point butterfly modules. | The full FFT is a mixed-language design. [cucapra/filament](https://github.com/cucapra/filament) is linked by the author; the exact XLS butterfly artifact was not located in current inspected directories. The arXiv identifier now has a revised title; this row deliberately cites v1. |
| [From Allo to XLS: Bridging Hardware Accelerator DSLs Through Traditional Compilation to ASIC Backends](https://www.cs.cornell.edu/courses/cs6120/2025fa/blog/allo-xls-backend/), Shao and Shyamsunder, December 2025 | GEMM variants and integer/float systolic arrays. | Public course report with [source backend](https://github.com/Nikil-Shyamsunder/allo-xls-backend/tree/main/allo/backend/xls). A useful design-generation source, not a peer-reviewed benchmark release. |
| [ABAX report](https://anthonybsong.github.io/resources/files/abax_report.pdf) | Another Allo backend effort targeting DSLX and XLS[cc]. | [AnthonyBSong/abax](https://github.com/AnthonyBSong/abax) README inspected. Separate generator lead; do not assume every bundled Allo kernel is supported by XLS. |
| [Integrating Support for Google XLS in hls4ml](https://indico.cern.ch/event/1496673/contributions/6622021/), Fast ML for Science 2025 | XLS backend for neural-network synthesis. | Current [official backend documentation](https://fastmachinelearning.org/hls4ml/backend/xls.html) and DSLX library were verified. Distinguish the 2025 presentation from subsequent backend development. |
| [SynCirc: Efficient Synthesis of Depth-Optimized Circuits from High-Level Languages](https://encrypto.de/papers/PSSSY26.pdf), extended paper | Secure-computation synthesis using XLS/Yosys, including advanced arithmetic and other circuit families. | [encryptogroup/SynCirc](https://github.com/encryptogroup/SynCirc) verified. Inspected lib and test/f32 largely contain Verilog, not corresponding DSLX sources. Recover original high-level inputs before labeling these XLS source designs. |
| [Enabling New HDLs with Agents](https://arxiv.org/abs/2501.00642), 2025 | DSLX among HDLAgent's generation targets. | [masc-ucsc/hdlagent](https://github.com/masc-ucsc/hdlagent) is archived. Benchmark/specification and generated-code lead; label model-generated provenance and avoid assuming every test has a released DSLX implementation. |
| [Porting software to hardware using XLS/DSLX](https://woset-workshop.github.io/PDFs/2021/a05.pdf), WOSET 2021 | HSV-to-RGB conversion on FoMU; slides also identify upstream FFT and numerical ports. | [Slides](https://woset-workshop.github.io/PDFs/2021/a05-slides.pdf) point to third_party/xls_colors and the FoMU workshop. Useful provenance and hardware-flow evidence. |
| [Explore the hardware synthesis accelerated by the open-source high-level synthesis tool, XLS](https://kth.diva-portal.org/smash/get/diva2%3A2055300/FULLTEXT01.pdf), KTH thesis lead, 2026 | Potential additional evaluation designs. | Search located the thesis, but the full PDF retrieval timed out. Design list, authorship details and code availability remain unverified. |

## 4. Smaller or adjacent sources

- [HLSFactory XLS test designs](https://github.com/sharc-lab/HLSFactory/tree/main/hlsfactory/hls_dataset_sources/test_designs_xls): 11 directories were verified. They cover arithmetic, arrays, pattern matching and procs. Useful for validating an ingestion pipeline; mostly below the requested complexity threshold. The presence of an XLS backend does not make its other HLS datasets automatically XLS-compatible.
- [xlsynth/dslx-llm](https://github.com/xlsynth/dslx-llm): prompts and acceptance tests, including arithmetic tasks. Keep specifications separate from completed implementations.
- [cdleary/xlsynth-proc-zoo](https://github.com/cdleary/xlsynth-proc-zoo): RAM-fetch/ReLU variants and send/receive micro-patterns. Useful for scheduling and backpressure coverage, low algorithmic diversity.
- [tilir/xls-tests](https://github.com/tilir/xls-tests): small CRC-oriented project; root directory verified.
- [XLS[cc] examples](https://github.com/google/xls/tree/main/xls/contrib/xlscc/examples): memory/delay, mux and loop examples. These are actual XLS C++ inputs, but mainly small demonstrations.
- Generic Vitis, CHStone, PolyBench, Rosetta and MachSuite repositories should not be tagged as existing XLS designs merely because they are HLS code.
- Compiler forks, mirrors, editor plugins and profiling utilities were excluded as independent design sources. The same caution applies to a repository that mentions DSLX only in its README.

## 5. Recommended ingestion rules

1. Start with P1 sources that have actual DSLX files and clear provenance: Zstandard, AES, FFT, numerical ports, SHA-256, IDCT, JPEG encoder, audio engine and FP32 systolic matmul.
2. Record one family identifier for related tops, pipeline configurations, bitwidths, generated variants and derivative repositories. Use family-level train/test splits to reduce leakage.
3. Keep source, generator and generated RTL as separate fields. For a mixed-language system, record exactly which part is DSLX.
4. Pin repository commit and compiler version; retain imports, build options, top function/proc, test inputs, licenses and reference models. XLS explicitly warns that DSLX compatibility changes over time. [Upstream guidance](https://github.com/google/xls).
5. Measure complexity after compilation: optimized IR nodes, operation mix, state bits, channels/procs, memory interfaces and synthesized area. Source length alone is misleading because DSLX often embeds tests and constants.
6. Keep availability and validation separate: source found; license identified; DSLX tests passed; IR conversion passed; codegen passed; RTL simulation passed; synthesis measured. Nothing in this discovery inventory has reached those execution-based stages.
7. Mark human-authored, AI-generated, generator-produced and unknown provenance explicitly. For the MASC accelerator and XLS32, AI participation is disclosed by the projects.
8. Prioritize unresolved artifact recovery for Histogram/Vector Core, Parafil butterflies, and the jet-tagging archive. The industrial datapaths in DATE 2024 are author-contact leads if public code cannot be found; no authors were contacted during this survey.

The accompanying JSON indexes **116 .x files across 16 selected inspected directories**, including blob hashes, byte sizes and direct URLs. It deliberately includes support files and tests where present: **116 files does not mean 116 independent designs**. It is a discovery aid, not a dependency-complete download manifest.

Search coverage included GitHub repository/README searches for DSLX, XLS[cc] and XLSynth; upstream source browsing; independent project inspection; web searches for algorithms, publications, theses and conference presentations; and following artifact references. GitLab/Codeberg-targeted web searches did not produce additional verified candidates. Global code search was not successfully used, and historical/nondefault branches and public discussion attachments have not been exhaustively traversed.


## 6. NEW XLS Designs

Added: September 21, 2026.

Unlike the discovery inventory above, everything in this section was **executed**,
not just located. Each design was fetched from upstream, configured for
HLSFactory, and run end to end through `XLSHLSSynthFlow` (DSLX → IR →
optimization → Verilog). All 10 pass.

Toolchain: XLS release `v0.0.0-10684-g998cf6aec`, sources pinned to the matching
commit `998cf6aec2a7b09c9ad14c492fe4fca63f831fb2`, so designs and compiler come
from one build. Run on Linux x86-64 (WSL2 Ubuntu 24.04), `pipeline_stages=1`,
`delay_model=unit`.

Dataset: `hlsfactory/hls_dataset_sources/xls_upstream/`. Regenerate with
`uv run python dev/fetch_xls_upstream.py`.

### The 10 new designs

| Design | Source | Top | Lines | Flops | Coverage added |
|---|---|---|---|---|---|
| rle_enc | [modules/rle/rle_enc.x](https://github.com/google/xls/blob/main/xls/modules/rle/rle_enc.x) | `RunLengthEncoder32` | 395 | 0 | Stateful stream compression, multi-module |
| fp_sincos_32 | [xls_go_math/fp_sincos_32.x](https://github.com/google/xls/blob/main/third_party/xls_go_math/fp_sincos_32.x) | `fp_sincos_32` | 266 | 96 | Taylor-series trigonometry |
| rle_dec | [modules/rle/rle_dec.x](https://github.com/google/xls/blob/main/xls/modules/rle/rle_dec.x) | `RunLengthDecoder32` | 244 | 0 | Decode counterpart to the encoder |
| reorder_queue | [examples/reorder_queue](https://github.com/google/xls/blob/main/xls/examples/reorder_queue/reorder_queue.x) | `reorder_queue_32_16` | 231 | 0 | Tag-ordered buffering, control-heavy |
| fpexp_32 | [xls_go_math/fpexp_32.x](https://github.com/google/xls/blob/main/third_party/xls_go_math/fpexp_32.x) | `fpexp_32` | 188 | 64 | Floating-point exponential |
| fp_trig_reduce | [xls_go_math/fp_trig_reduce.x](https://github.com/google/xls/blob/main/third_party/xls_go_math/fp_trig_reduce.x) | `fp_trig_reduce_32` | 114 | 67 | Payne–Hanek argument reduction |
| lfsr | [examples/lfsr.x](https://github.com/google/xls/blob/main/xls/examples/lfsr.x) | `lfsr8` | 82 | 16 | Linear-feedback shift register |
| serialized_decomposer | [examples/serialized_decomposer.x](https://github.com/google/xls/blob/main/xls/examples/serialized_decomposer.x) | `serialized_decomposer` | 73 | 4 | Channel serialization proc |
| sparse_prefix_sum | [examples/sparse_prefix_sum.x](https://github.com/google/xls/blob/main/xls/examples/sparse_prefix_sum.x) | `sparse_prefix_sum` | 55 | 512 | Sparse scan variant |
| prefix_sum | [examples/prefix_sum.x](https://github.com/google/xls/blob/main/xls/examples/prefix_sum.x) | `prefix_sum` | 34 | 512 | Dense parallel scan |

Licenses: `xls/` designs are Apache-2.0; the `third_party/xls_go_math` ports
carry their own BSD-3 license, retained per rule 4 above. Each design directory
holds a `PROVENANCE.json` with upstream path, commit, release and license.

These join 9 designs ingested earlier the same way — `riscv_simple` (795),
`idct_chen` (322), `hack_cpu` (278), `fpsqrt_32` (205), `fpdiv_2x32` (170),
`sha256` (136), `cubic_bezier` (48), `adler32` (35), `crc32` (28).

### Section 1 designs

A third pass went after the substantial families in section 1 that live inside
`google/xls`. Six more designs, all passing:

| Design | Source | Top | Lines | Flops | Notes |
|---|---|---|---|---|---|
| aes_key_schedule | [modules/aes/aes.x](https://github.com/google/xls/blob/main/xls/modules/aes/aes.x) | `create_key_schedule` | 296 | 2178 | AES-128 key expansion |
| aes_ctr_encrypt | [modules/aes/aes_ctr.x](https://github.com/google/xls/blob/main/xls/modules/aes/aes_ctr.x) | `aes_ctr_encrypt` | 237 | 642 | Counter mode |
| aes_encrypt | [modules/aes/aes.x](https://github.com/google/xls/blob/main/xls/modules/aes/aes.x) | `encrypt` | 296 | 514 | Block encryption |
| aes_decrypt | [modules/aes/aes.x](https://github.com/google/xls/blob/main/xls/modules/aes/aes.x) | `decrypt` | 296 | 514 | Block decryption |
| aes_ghash_gf128_mul | [modules/aes/ghash.x](https://github.com/google/xls/blob/main/xls/modules/aes/ghash.x) | `gf128_mul` | 350 | 384 | GF(2^128) multiply for GCM |
| ws_matmul | [modules/ml/ws_matmul.x](https://github.com/google/xls/blob/main/xls/modules/ml/ws_matmul.x) | `ws_2x2` | 536 | 0 | Weight-stationary FP32 systolic array, 4 stages |

Per rule 2 above, the five AES entries share one family identifier for
train/test splitting.

**Totals: 25 designs, 5,710 lines of DSLX, 25/25 passing.** For comparison, the
pre-existing `test_designs_xls` is 11 designs totalling 87 lines.

### Where these were found

All from the upstream `google/xls` repository, in three places:

- `xls/examples/` — standalone kernels. Most are single-file and stdlib-only.
- `xls/modules/` — larger multi-file families. The RLE pair came from here; the
  Zstandard and AES families in section 1 live here too.
- `third_party/` — ported numerical libraries with their own licenses.

Candidates were enumerated from the directory listings at the pinned commit,
then filtered on two criteria that turn out to decide ingestibility (below).

### Four execution findings

Discovery alone does not predict whether a design can be ingested. Four things
only became visible by running the tools.

**1. A flow bug: stdlib imports were broken.** `ir_converter_main` defaults
`--dslx_stdlib_path` to the *relative* path `xls/dslx/stdlib`, which resolves
only when the tool runs from the installation root. `XLSHLSSynthFlow` runs with
`cwd` set to the design directory and never passed the flag, so **every design
containing `import std` failed to convert**. It had gone unnoticed because none
of the 11 pre-existing test designs import anything — the flow had never been
exercised against an import. Fixed via `find_dslx_stdlib()` in `flow_xls.py`.

**2. Parametric tops are not synthesizable.** A DSLX parametric `fn` or `proc`
cannot be a `top` without explicit instantiation. This excludes otherwise
attractive candidates whose only concrete entities are `#[test]` functions:
`bitonic_sort.x`, `fir_filter.x`, `apfloat_fmac.x`, and `sobel_filter.x` — whose
`apply_stencil_float32` and `sobel_filter_float32` are both parametric, leaving
only the trivial helper `convert_triplet`. Ingesting these means authoring a
concrete wrapper, which is a different activity from ingesting upstream code.
**Source size is a poor predictor here; the shape of the top is what matters.**

**3. Procs need an explicit reset.** Proc state registers carry reset values, and
codegen fails with `Register ... has a reset value but corresponding register
write operation ... has no reset operand` unless `reset` is set. The four proc
designs above all set `reset = "rst"`.

**4. Section 1 splits on function vs. proc tops, not on size.** AES turned out
to be straightforward: `encrypt`, `decrypt`, `create_key_schedule`, `gf128_mul`
and `aes_ctr_encrypt` are all concrete *functions*, so the whole family is
reachable with `dslx_path` and nothing else. That is the highest-value tier in
this survey, and it needed no new flow capability.

**5. Some proc networks work, some don't — and the reason is per-design.**
`matmul_4x4` was attempted and **dropped**: it schedules only with more stages,
then fails with `Channel _node_weights__0_0 has no fifo config`. An earlier
draft of this section generalised that to "proc networks are blocked", which was
**wrong** — `ws_matmul`'s `ws_2x2` proc network converts, optimises and
generates RTL at 4 stages. The FIFO issue is specific to how `matmul_4x4`
declares its channels.

**6. Zstandard: 60 modules convert and optimise; codegen is the open question.**
With `resolve_imports=True` the fetch script vendored the full 60-module,
35,803-line transitive closure of `zstd_dec.x`, and `ir_converter_main` plus
`opt_main` succeeded — a 4.19 MB optimised IR in ~34 seconds. Codegen then
reported `cannot achieve full throughput. Try --worst_case_throughput=2`. That
setting was added to the flow, after which codegen fails on a further scheduling
constraint not yet diagnosed. The decoder is **not** in the dataset; it is one
or two codegen flags away rather than blocked on anything structural.

### A new flow capability: `dslx_path`

Designs importing modules beyond the stdlib previously could not be ingested at
all. `XLSHLSSynthFlow` now accepts a `dslx_path` setting, and imported modules
are vendored into the design directory under their repository-relative path so a
dotted import such as `third_party.xls_go_math.fp_trig_reduce` resolves against
the design itself:

```
xls_upstream/rle_enc/
├── rle_enc.x
├── xls/modules/rle/rle_common.x     <- import root, dslx_path = "."
├── hlsfactory.toml
└── PROVENANCE.json
```

Designs stay self-contained and portable, which matters because HLSFactory
copies each design directory into the work directory before running. Ten of the
25 depend on this: the four in the second batch plus all five AES designs.

For deep dependency trees, `resolve_imports=True` in `dev/fetch_xls_upstream.py`
walks the transitive import closure automatically instead of listing every
module by hand. It was validated on Zstandard's 60-module tree.

Two further flow settings were added alongside `dslx_path`, both surfaced by
real failures: `reset` was already supported; `worst_case_throughput` (relaxes
the scheduler's full-throughput requirement) is new.

### Caveats

- `pipeline_stages=1` for all but `ws_matmul` (4), so most latencies read 2 and the
  numbers are a single design point per kernel, not a characterization. Sweeping stages and
  delay models is the obvious next step and is nearly free.
- The proc designs report `flops=0` or `4`, which reflects the degenerate
  schedule rather than the real cost of the design.
- Verified on WSL2 Ubuntu 24.04, **not** on RHEL 8.10. The flow fix is
  platform-independent, but whether the prebuilt binaries run against the lab
  server's older glibc is still unconfirmed.
- Passing the flow means DSLX converted, optimized and generated Verilog. It does
  **not** mean DSLX tests pass, RTL simulates correctly, or synthesis succeeds —
  the later stages of rule 6 above remain unreached.
