# GIN Virtual Node Python code

This directory contains the PyTorch implementation of GNN models (GIN, GIN-Virtual, GCN, GCN-Virtual). The PyTorch implementation is used to generate pre-trained weights and golden output against which our GoldenC implementation will be verified.

The PyTorch implementation is dervied from: [GNN_baselines](https://github.com/snap-stanford/ogb/tree/master/examples/graphproppred/mol)
We use ogbg-molhiv dataset for graph property prediction.

## Setting up the environment:

- Install miniconda and cuda -> Follow steps in /usr/scratch/README.md @chao-srv1.ece.gatech.edu 
- Create a virtual enevironment -> conda create -n gin_virtual_node
- source activate gin_virtual_node
- conda install -n gin_virtual_node pytorch torchvision torchaudio cudatoolkit=11.1 -c pytorch -c nvidia
- conda install -n gin_virtual_node pyg -c pyg -c conda-forge 
- conda install -n gin_virtual_node -c conda-forge ogb

## Generating the pre-trained weights:

- Train the model with 5 layers and embedding dimension=100 using ogbg-molhiv dataset for 1 epoch using the following command:
<p align="center"> 
python main_pyg.py --gnn gin-virtual --emb_dim 100 --num_layer 5 --epochs 1 --dataset ogbg-molhiv
</p>

- Running the above command will generate the trained model weights gin-virtual_ep1_dim100.pt which will be used to prepare weights for golden C and accelerator

## Prepare weights for Golden C and accelerator:

- Run the script prepare_weights.py
- Input to prepare_weights.py: Pre-trained model weights obtained in the above step gin-virtual_ep1_dim100.pt
- The above pre-trained model has batch norm layer to speedup training. However, batchnorm layer introduces complexity in Hardware. Thus, weight fusin is applied wherein weights of the linear layer are merged with the weights of the batchnorm layer. The resulting model is labelled as _noBN_ model which provides the same output as the model with Batchnorm. 
- Output of prepare_weights.py:
    - Pre-trained model without batchnorm after weight fusion: gin-virtual_ep1_noBN_dim100.pt
    - Golden output from PyTorch to be used for GoldenC verification: Pytorch_virtual_node_noBN_output_dim100.txt
    - All weights with corresponding offset used to automate load weights function in Golden C (used by gen_c.py): gin-virtual_ep1_noBN_dim100.weights.dict.json
    - All weight binaries necessary for inference in GoldenC and accelerator.
