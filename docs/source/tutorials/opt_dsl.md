(opt_dsl_tutorial)=
# OptDSL Frontend for Design Space Expansion

OptDSL is a design-space exploration frontend that automatically generates multiple HLS design variants by applying different optimization directives (pragmas) to a base design. Instead of manually creating dozens of versions with different unroll factors, partition types, or pipeline settings, you define the design space once in an `opt_template.tcl` file and OptDSL enumerates or samples the combinations.

**When to use OptDSL:** Use it when you want to explore many pragma combinations for ML training, benchmarking, or design-space studies. Designs must include an `opt_template.tcl` file that specifies the design space (see [OptDSL](../framework/optdsl) for syntax). Built-in datasets like PolyBench, MachSuite, and CHStone already include OptDSL templates.

**Minimal example:**

```python
from hlsfactory.opt_dsl_frontend import OptDSLFrontend

opt_dsl_frontend = OptDSLFrontend(
    work_dir=WORK_DIR,
    random_sample=True,
    random_sample_num=16,
    random_sample_seed=42,
    log_execution_time=True,
)

datasets_post_frontend = opt_dsl_frontend.execute_multiple_design_datasets_fine_grained_parallel(
    datasets,
    copy_dataset=True,
    new_dataset_name_fn=lambda x: f"{x}__post_frontend",
    n_jobs=32,
    cpu_affinity=list(range(32)),
)
```

`random_sample=True` limits the number of generated designs per base design; without it, full enumeration can produce hundreds or thousands of variants. The output is a new `DesignDatasetCollection` that you can feed into synthesis flows (e.g., Xilinx or Intel).

For OptDSL syntax (v1 and v2), configuration options, and usage examples, see the [OptDSL framework guide](../framework/optdsl).
