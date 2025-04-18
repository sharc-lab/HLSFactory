# Write out expected activations for each layer of resnet18
# python get_exp_acts.py <input_image> <output_dir>

import torchvision
from torchvision.io import read_image
from functools import partial
import os, sys
import numpy as np
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('inp_fname', type=str, help='Input image file name')
parser.add_argument('op_dir', type=str, help='Output directory for bin files')
args = parser.parse_args()

inp_fname = args.inp_fname
op_dir = args.op_dir

weights = torchvision.models.ResNet18_Weights.IMAGENET1K_V1
model = torchvision.models.resnet18(weights=weights)

def save_op_hook(module, inp, outp, fname=None):
    module.last_outp = outp
    if fname is not None:
        act = outp.detach().squeeze(0).numpy().astype(np.float32)
        print(fname.split('/')[-1].split('.')[0],'\t', act.shape)
        act.tofile(fname)

os.system(f"mkdir -p {op_dir}")

# Setup hooks
model.relu.register_forward_hook(partial(save_op_hook, fname=os.path.join(op_dir, "conv1_out.bin")))
model.maxpool.register_forward_hook(partial(save_op_hook, fname=os.path.join(op_dir, "maxpool_out.bin")))

model.layer1[0].relu.register_forward_hook(partial(save_op_hook, fname=os.path.join(op_dir, "l10_c1_out.bin")))
model.layer1[0].register_forward_hook(partial(save_op_hook, fname=os.path.join(op_dir, "l10_c2_out.bin")))
model.layer1[1].relu.register_forward_hook(partial(save_op_hook, fname=os.path.join(op_dir, "l11_c1_out.bin")))
model.layer1[1].register_forward_hook(partial(save_op_hook, fname=os.path.join(op_dir, "l11_c2_out.bin")))

model.layer2[0].relu.register_forward_hook(partial(save_op_hook, fname=os.path.join(op_dir, "l20_c1_out.bin")))
model.layer2[0].register_forward_hook(partial(save_op_hook, fname=os.path.join(op_dir, "l20_c2_out.bin")))
model.layer2[0].downsample.register_forward_hook(partial(save_op_hook, fname=os.path.join(op_dir, "l2_ds_out.bin")))
model.layer2[1].relu.register_forward_hook(partial(save_op_hook, fname=os.path.join(op_dir, "l21_c1_out.bin")))
model.layer2[1].register_forward_hook(partial(save_op_hook, fname=os.path.join(op_dir, "l21_c2_out.bin")))

model.layer3[0].relu.register_forward_hook(partial(save_op_hook, fname=os.path.join(op_dir, "l30_c1_out.bin")))
model.layer3[0].register_forward_hook(partial(save_op_hook, fname=os.path.join(op_dir, "l30_c2_out.bin")))
model.layer3[0].downsample.register_forward_hook(partial(save_op_hook, fname=os.path.join(op_dir, "l3_ds_out.bin")))
model.layer3[1].relu.register_forward_hook(partial(save_op_hook, fname=os.path.join(op_dir, "l31_c1_out.bin")))
model.layer3[1].register_forward_hook(partial(save_op_hook, fname=os.path.join(op_dir, "l31_c2_out.bin")))

model.layer4[0].relu.register_forward_hook(partial(save_op_hook, fname=os.path.join(op_dir, "l40_c1_out.bin")))
model.layer4[0].register_forward_hook(partial(save_op_hook, fname=os.path.join(op_dir, "l40_c2_out.bin")))
model.layer4[0].downsample.register_forward_hook(partial(save_op_hook, fname=os.path.join(op_dir, "l4_ds_out.bin")))
model.layer4[1].relu.register_forward_hook(partial(save_op_hook, fname=os.path.join(op_dir, "l41_c1_out.bin")))
model.layer4[1].register_forward_hook(partial(save_op_hook, fname=os.path.join(op_dir, "l41_c2_out.bin")))

model.avgpool.register_forward_hook(partial(save_op_hook, fname=os.path.join(op_dir, "avgpool_out.bin")))
model.fc.register_forward_hook(partial(save_op_hook, fname=os.path.join(op_dir, "fc_out.bin")))

model.eval()

preprocess = weights.transforms()
i = read_image(inp_fname)
img = preprocess(i).unsqueeze(0)

# Save preprocessed input
inp = img.detach().squeeze(0).numpy().astype(np.float32)
print('input\t', inp.shape)
inp.tofile(os.path.join(op_dir, "input.bin"))

# Forward pass
op = model(img)[0]
class_id = op.argmax().item()
category_name = weights.meta["categories"][class_id]
print(f"Prediction: {class_id}, {category_name}")
