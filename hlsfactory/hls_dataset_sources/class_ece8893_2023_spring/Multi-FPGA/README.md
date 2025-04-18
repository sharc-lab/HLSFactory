This is the code for our cross board communication demo.

The functions deployed on two boards are tiled convolution and ranged batchnorm respectively. Their source code is in folders 'Tile_Conv' and 'RBN'. To acquire the IPs needed in Vivado block design, run hls with the two script within the folder.

The 'sim.cpp' file in Tile_Conv is the testbench file we used to verify correctness of streaming data behavior.

The part we used is xczu9eg-ffvb1156-2-e

Large portion of work with Aurora is done through Vivado block design. As mentioned in our report and presentation, we need data width converter, FIFO and Aurora 64b_66b modules included in design. 

In Aurora_64b_66b, under 'Shared Logic', 'Include Shared Logic in core' should be selected. 'Dataflow Mode' should be specified as 'RX-only' SImplex or 'TX-only' SImplex respectively. 'Starting GT Quad' and 'Starting GT Lane' should also be specified according to the ports used to plug cords.

After obtaining the two sets of .bit and .hwh files, another modified hwh file will be needed. The python script 'hwh_new.py' will perform the transformation. Then design is ready to go on board. Python version >3.6 maybe needed here.
