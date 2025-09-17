from concrete_design.design import ConcreteDesign

# design = ConcreteDesign("../0_examples/0_concrete_design", "../0_work_dir/0_concrete_design")
design = ConcreteDesign("/home/cy/github/DSE-HLS-FFT/0_work_dir/dse_vitishlssyn_3mm/dse_resource_latency/work_0", "/home/cy/github/DSE-HLS-FFT/0_work_dir/dse_vitishlssyn_3mm/dse_resource_latency/work_0")

# design = ConcreteDesign("../0_examples/1_multi_file_design", "../0_work_dir/1_multi_file_design", force_overwrite=False)

# result = design.run_vitis_hls(timeout_min=5)
# if result == "success":
#     print("HLS completed successfully!")


# design.analyze_source()
# design.analyze_csynth()
# design.map_c_to_rtl()

design.run_all_analyses()





