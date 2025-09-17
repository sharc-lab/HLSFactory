kernels=(
    "BU"
    "dual_interface_FIFO_receive"
    "dual_interface_FIFO_send"
    "fwd_load_inv_store_poly_1_limbs"
    "fwd_store_inv_load_poly_1_limbs"
    "input_selector_single"
    "Mmap2Stream_tf_0_1_limbs"
    "NTT_kernel"
    "output_slector_single"
    "shuffler_buf"
    "shuffler_in"
    "shuffler_out_shift"
    "shuffler_out_shuff"
    "TFBuf_wiFW_0"
    "TFBuf_wiFW_1"
    "TFBuf_wiFW_2"
    "TFBuf_woFW_3"
)

gen_dir="generated_kernels"
if [ "$gen_dir" ]; then
    rm -rf "$gen_dir"
fi
mkdir -p "$gen_dir"

# run_hls:
# 	@if [ -z "$(target)" ]; then \
# 		echo "Error: You must provide target=..."; \
# 		exit 1; \
# 	fi
# 	if [ -d "$(target)_project" ]; then \
# 		rm -rf "$(target)_project"; \
# 	fi;
# 	mkdir $(target)_project
# 	cd $(target)_project; \
# 	cp -rf ../tcl_template/run_hls.tcl .; \
# 	ln -sf ../cpp/$(target).cpp .; \
# 	ln -sf ../cpp/ntt.h .; \
# 	ln -sf ../src .; \
# 	sed -i 's/@MODULE_NAME@/$(target)/g' run_hls.tcl; \
# 	# vitis_hls -f run_hls.tcl

for target in "${kernels[@]}"; do
    if [ -d "$gen_dir/${target}_project" ]; then
        rm -rf "$gen_dir/${target}_project"
    fi
    mkdir -p "$gen_dir/${target}_project"
    cd "$gen_dir/${target}_project"
    cp -rf ../tcl_template/run_hls.tcl .
    # dont link but copy
    cp ../cpp/"${target}".cpp .
    cp ../cpp/ntt.h .
    cp -r ../src .
    sed -i "s/@MODULE_NAME@/${target}/g" run_hls.tcl
    # vitis_hls -f run_hls.tcl

done