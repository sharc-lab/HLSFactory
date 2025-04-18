1. LG_Conv and Simple_Conv are the two convolution layers implemented for our project. We have integrated these layers to the gnnbuilder framework which can be found in the github link https://github.com/stefanpie/gnn-builder

2. conv_layer/Python/gnn_test_data.py imports the layers from pytorch and genereates the test data which acts as the golden reference for the comparison

3. conv_layer/HLS/gnn_builder_lib.h has the above mentioned layers implemented in C and optimized using HLS pragmas.

4. Both the files are plugged in to the gnn builder framework present in the above mentioned github link and verified for the functionality.

5. Floating point and fixed point simulations were verified with a maximum error of 1 x e-3.

6. LG_Conv Code : https://github.com/stefanpie/gnn-builder/tree/FPGA_Project_Hari

7. Simple_Conv Code : https://github.com/stefanpie/gnn-builder/tree/FPGA_project_Naga