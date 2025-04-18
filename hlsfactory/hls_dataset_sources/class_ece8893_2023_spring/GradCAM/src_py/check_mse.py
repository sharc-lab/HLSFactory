import numpy as np
import os
py_out_path  = 'expected_activations/n01739381_vine_snake/'
cpp_out_path = 'src_hls/out/'

file_list = [
    "input.bin",
    "conv1_out.bin",
    "maxpool_out.bin",
    #"l10_c1_out.bin",
    "l10_c2_out.bin",
    #"l11_c1_out.bin",
    "l11_c2_out.bin",
    "l2_ds_out.bin",
    #"l20_c1_out.bin",
    "l20_c2_out.bin",
    #"l21_c1_out.bin",
    "l21_c2_out.bin",
    "l3_ds_out.bin",
    #"l30_c1_out.bin",
    "l30_c2_out.bin",
    #"l31_c1_out.bin",
    "l31_c2_out.bin",
    "l4_ds_out.bin",
    #"l40_c1_out.bin",
    "l40_c2_out.bin",
    #"l41_c1_out.bin",
    "l41_c2_out.bin",
    "avgpool_out.bin",
    "output.bin",
    "cam_output.bin"
]

PASS = True
for file_name  in file_list:
    cpp_output = np.fromfile(os.path.join(cpp_out_path, file_name), dtype=np.float32)
    py_output  = np.fromfile(os.path.join(py_out_path,  file_name), dtype=np.float32)

    mse = (np.square(py_output - cpp_output)).mean()
    max_err = np.max(np.abs((py_output - cpp_output) / (py_output + 1e-20)))
    max_err_idx = np.argmax(np.abs((py_output - cpp_output) / (py_output + 1e-20)))
    print(f"Checking {file_name}")
    print(f"Size: {cpp_output.size} == {py_output.size}")
    print(f"MSE: {mse}")
    #print(f"Max error: {max_err}, @ {cpp_output[max_err_idx]} - {py_output[max_err_idx]}")
    #print(py_output[:10])
    #print(cpp_output[:10])
    print()

    if mse > 1e-8:
        PASS = False

    if file_name == "output.bin":
        print(f"Expected classificiation: {np.argmax(py_output)}")
        print(f"Actual classificiation: {np.argmax(cpp_output)}")
        print()

print("PASS" if PASS else "FAIL")
