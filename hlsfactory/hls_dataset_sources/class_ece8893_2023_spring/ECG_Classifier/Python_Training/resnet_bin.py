import torch
import torch.nn as nn
import torchvision
import torchvision.transforms as transforms
#from resnet import ResNet18

import Model

import numpy
import re
from typing import Dict,Iterable, Callable

model   = Model.Model()
trained_model_path  = "/Users/daniela/Library/CloudStorage/OneDrive-GeorgiaInstituteofTechnology/Spring 2023/FPGA/Project/model_fold_6.pth"
model.load_state_dict(torch.load(trained_model_path,map_location=torch.device('cpu')))
model.eval()

features = {}
def get_features(name):
    def hook(model, input, output):
        features[name] = output.cpu().detach()
    return hook

raw_layers = []
for layer in model.named_modules():
    raw_layers.append(layer[0])

print("\n\nRaw Layers:\n",raw_layers)

leaf_layers = []
for i in range(1, len(raw_layers)-1):
    curr_layer = raw_layers[i]
    next_layer = raw_layers[i+1]
    if(next_layer[:len(curr_layer)+1] != curr_layer + "."):
        leaf_layers.append(curr_layer)

leaf_layers.append(next_layer)
print("\n\nLeaf Layers:\n", leaf_layers)

layers = []
for i in range(len(leaf_layers)):
    layers.append(re.sub(r"\.(\d)",r"[\1]",leaf_layers[i]))
    
for i in range(len(layers)):
    layer = layers[i]
    layer_hook = "model." + layer + ".register_forward_hook(get_features('" + layer + "'))"
    exec(layer_hook)

print("\n\nLayers:\n", layers)

# output needs to be evaluated after adding the layer_hook
inp = torch.ones((1,1,187), requires_grad=True)
outp = model(inp)   

print(model.state_dict().keys())

write_layer_outputs_to_file         = True
write_fused_layer_params_to_file    = True
write_layer_params_to_file          = True

EPS = 10 ** -5 # constant

if(write_layer_outputs_to_file):
    # Write layer outputs
    for i in range(len(layers)):
        layer = layers[i]
        print("Outputs ",layer)
        #print('HEREEEE')
        if(layer in features.keys()):
            print('LAYER NAME:',layer)
            layer_name = layer.replace("].","_")
            layer_name = layer_name.replace("[", "_")
            layer_name = layer_name.replace("]", "")
            filename = "bin/" +  layer_name + ".bin"
            with open(filename,"wb") as f:
                features[layer].cpu().numpy().tofile(f)
            print("Layer " + str(i) + " feature map printed to " + filename)
            print("Shape:", features[layer].cpu().numpy().shape)

if(write_layer_params_to_file):
    # Write layer params
    for i in range(len(layers)):
        layer = layers[i]
        print(layer)
        #if('conv' in layer or 'downsample[0]' in layer):
        if('conv' in layer or 'downsample[0]' in layer or 'dense' in layer):
            layer_name = layer.replace("].","_")
            layer_name = layer_name.replace("[", "_")
            layer_name = layer_name.replace("]", "")
            filename = "bin/" +  layer_name + "_weights.bin"
            
            param_name = layer.replace("[",".")
            param_name = param_name.replace("]","")
            param = model.state_dict()[param_name+'.weight'].detach().numpy()
            print('weight shape:',model.state_dict()[param_name+'.weight'].detach().numpy().shape)
            with open(filename, "wb") as f:
                param.tofile(f)
            print("Param " + param_name + " printed to file " + filename)

        if('bn' in layer or 'downsample[1]' in layer):
            layer_name = layer.replace("].","_")
            layer_name = layer_name.replace("[", "_")
            layer_name = layer_name.replace("]", "")
            filename = "bin/" +  layer_name + "_params.bin"
      
            param_name = layer.replace("[",".")
            param_name = param_name.replace("]","")
      
            weight      = model.state_dict()[param_name+'.weight']
            sqrt_var    = torch.sqrt(model.state_dict()[param_name+'.running_var'] + EPS)
            wt_sqrt_var = weight / sqrt_var
      
            param = numpy.concatenate([
                wt_sqrt_var.detach().numpy(),
                model.state_dict()[param_name+'.bias'].detach().numpy(),
                model.state_dict()[param_name+'.running_mean'].detach().numpy()])

            with open(filename, "wb") as f:
                param.tofile(f)
            print("Param " + param_name + " printed to file " + filename)

if(write_fused_layer_params_to_file):
    # Write layer params
    for i in range(len(layers)):

        layer = layers[i]

        print(layer)
        print('here')
        #if('linear' in layer):
        #if('conv' in layer):
        if('conv' in layer or 'dense' in layer):
            #print('test inside IF')
            linear_weight = model.state_dict()[layer+'.weight'].detach().numpy()
            filename = "bin/" + layer + "_weights.bin"
            with open(filename, "wb") as f:
                linear_weight.tofile(f)
            print("Param " + layer + "weights"+ " printed to file " + filename)
            print("Shape:", linear_weight.shape)
            
            linear_bias = model.state_dict()[layer+'.bias'].detach().numpy()
            filename = "bin/" + layer + "_bias.bin"
            with open(filename, "wb") as f:
                linear_bias.tofile(f)
            print("Param " + layer + "bias"+ " printed to file " + filename)
            print("Shape bias:", linear_bias.shape)
            
        
        if('conv' in layer or 'downsample[0]' in layer or 'shortcut[0]' in layer):
            conv_layer_name = layer.replace("].","_")
            conv_layer_name = conv_layer_name.replace("[", "_")
            conv_layer_name = conv_layer_name.replace("]", "")

            conv_param_name = layer.replace("[",".")
            conv_param_name = conv_param_name.replace("]","")

            conv_weight = model.state_dict()[conv_param_name+'.weight']

        if('bn' in layer or 'downsample[1]' in layer or 'shortcut[1]' in layer):
            bn_layer_name = layer.replace("].","_")
            bn_layer_name = bn_layer_name.replace("[", "_")
            bn_layer_name = bn_layer_name.replace("]", "")

            bn_param_name = layer.replace("[",".")
            bn_param_name = bn_param_name.replace("]","")

            bn_weight = model.state_dict()[bn_param_name+'.weight']
            bn_bias   = model.state_dict()[bn_param_name+'.bias']
            bn_mean   = model.state_dict()[bn_param_name+'.running_mean']
            bn_var    = model.state_dict()[bn_param_name+'.running_var']

            bn_factor    = torch.div(bn_weight,torch.sqrt(bn_var+EPS)).view(-1,1,1,1)
            fused_weight = torch.mul(conv_weight, bn_factor)
            fused_bias   = bn_bias - torch.div(torch.mul(bn_weight,bn_mean),torch.sqrt(bn_var+EPS))
            
            if('downsample' in bn_layer_name):
                layer_number = '0'
                layer_prefix = bn_layer_name[0:bn_layer_name.find('downsample')]
            else:
                layer_number = conv_layer_name[-1]
                layer_prefix = bn_layer_name[0:bn_layer_name.find('bn')]
        
            weights_filename = "bin/fused_" + layer_prefix + "conv" + layer_number + "_bn" + layer_number + "_weights.bin"
            bias_filename    = "bin/fused_" + layer_prefix + "conv" + layer_number + "_bn" + layer_number + "_bias.bin"
            with open(weights_filename, "wb") as f:
                fused_weight.detach().numpy().tofile(f)
            print("Fused weights of " + layer_prefix + layer_number + " printed to file " + weights_filename)
            print("Shape:", fused_weight.shape)
        
            with open(bias_filename, "wb") as f:
                fused_bias.detach().numpy().tofile(f)
            print("Fused biases of " + layer_prefix + layer_number + " printed to file " + bias_filename)
            print("Shape:", fused_bias.shape)