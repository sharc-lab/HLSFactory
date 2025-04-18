import torch
import math
import numpy as np

def convert_to_n_ary(dec_matrix, base, bits=8):
    # expand each column in the decimal matrix to an n-ary number
    rows, cols = dec_matrix.shape
    dec_matrix = dec_matrix.flatten().reshape(-1,1).int()

    max_val = 2**bits
    num_digits = math.ceil(math.log(max_val, base))

    n_ary = base**torch.arange(num_digits, device='cuda').flip(0)

    out = dec_matrix // n_ary % base

    return out.reshape(rows, num_digits*cols)

def map_weights(args, weights, original_shape):
    rows_PE = math.ceil(weights.shape[0] / args.sub_array[0])
    cols_PE = math.ceil(weights.shape[1] / args.sub_array[1])

    with open('PE_sizes.txt', 'ab') as f:
        f.write(f'weight shape: {original_shape[0]}x{original_shape[1]} converted shape: {weights.shape[0]}x{weights.shape[1]} PE shape: {rows_PE}x{cols_PE}')
