import torch 
import torch.nn as nn
import torchvision
import torchvision.transforms as transforms
from torchsummary import summary

import numpy as np

device  = torch.device('cpu')
print("Using", device)
batch_size      = 64
num_classes     = 10

class ConvNeuralNet(nn.Module):

    def __init__(self, num_classes):
        super(ConvNeuralNet, self).__init__()
        self.conv_layer1    = nn.Conv2d(in_channels=3, out_channels=32, padding='same', kernel_size=3)
        self.conv_layer2    = nn.Conv2d(in_channels=32, out_channels=32, padding='same', kernel_size=3)
        self.max_pool1      = nn.MaxPool2d(kernel_size=2, stride=2)

        self.conv_layer3    = nn.Conv2d(in_channels=32, out_channels=64, padding='same', kernel_size=3)
        self.conv_layer4    = nn.Conv2d(in_channels=64, out_channels=64, padding='same', kernel_size=3)
        self.max_pool2      = nn.MaxPool2d(kernel_size=2, stride=2)

        self.fc1            = nn.Linear(4096, 128)
        self.relu1          = nn.ReLU()
        self.fc2            = nn.Linear(128, num_classes)


    def forward(self, x):
        out     = self.conv_layer1(x)
        out     = self.conv_layer2(out)
        out     = self.max_pool1(out)

        out     = self.conv_layer3(out)
        out     = self.conv_layer4(out)
        out     = self.max_pool2(out)

        out     = out.reshape(out.size(0), -1)

        out     = self.fc1(out)
        out     = self.relu1(out)
        out     = self.fc2(out)

        return out

num_classes = 10
PATH = './cifar10_net.pth'
model   = ConvNeuralNet(num_classes)
model.to(device)
model.load_state_dict(torch.load(PATH))
model.eval()

features = {}

def get_features(name):
  def hook(model, input, output):
    features[name] = output.cpu().detach()
  return hook

layers = []
for layer in model.named_modules():
    layers.append(layer[0])

layers = layers[1:]
print(layers)
for i in range(len(layers)):
  layer = layers[i]
  print(layer)
  layer_hook = "model." + layer + ".register_forward_hook(get_features('" + layer + "'))"
  exec(layer_hook)


inp = torch.ones(1,3,32,32, requires_grad=True)
inp.to(device)
outp = model(inp)
print(features.keys())

b = model.state_dict()
layer_names = b.keys()

def conv_operation(in_tensor, wt_tensor, bias_tensor):
    in_shape = np.shape(in_tensor)
    wt_shape = np.shape(wt_tensor)

    out_tensor = np.zeros((in_shape[0],wt_shape[0],in_shape[2],in_shape[3]))

    for oc in range(wt_shape[0]):
        for oh in range(in_shape[2]):
            for ow in range(in_shape[3]):
                for ic in range(in_shape[1]):
                    for kh in range(wt_shape[2]):
                        for kw in range(wt_shape[3]):
                            if(oh+kh-1 <0 or ow+kw-1 <0 or oh+kh > in_shape[2] or ow+kw > in_shape[3]):
                                current_ip = 0
                            else:
                                current_ip = in_tensor[0][ic][oh+kh-1][ow+kw-1]

                            if(ic == 0 and kh==0 and kw==0 ):
                                out_tensor[0][oc][oh][ow] = current_ip * wt_tensor[oc][ic][kh][kw] + bias_tensor[oc]
                            else:
                                out_tensor[0][oc][oh][ow] += current_ip * wt_tensor[oc][ic][kh][kw]

    return out_tensor

def max_pool(in_tensor):
    in_shape = np.shape(in_tensor)
    out_shape = (in_shape[0], in_shape[1], int(in_shape[2]/2), int(in_shape[3]/2))

    # print(out_shape)
    out_tensor = np.zeros(out_shape)

    for ic in range(in_shape[1]):
        for ih in range(0,in_shape[2],2):
            for iw in range(0,in_shape[3],2):
                out_tensor[0][ic][int(ih/2)][int(iw/2)] = max(in_tensor[0][ic][ih][iw],
                                                    in_tensor[0][ic][ih+1][iw],
                                                    in_tensor[0][ic][ih][iw+1],
                                                    in_tensor[0][ic][ih+1][iw+1])

    return out_tensor

def conv_flatten_fc(in_tensor, wt_tensor, bias_tensor):
    in_shape = np.shape(in_tensor)
    wt_shape = np.shape(wt_tensor)

    out_tensor = np.zeros((1, wt_shape[0]))

    for i in range(wt_shape[0]):
        out_tensor[0][i] = bias_tensor[i]
        for oc in range(in_shape[1]):
            for oh in range(in_shape[2]):
                for ow in range(in_shape[3]):
                    out_tensor[0][i] += in_tensor[0][oc][oh][ow] * wt_tensor[i][in_shape[1]*oc + in_shape[2]*oh + ow]

    return out_tensor

def relu_fc(in_tensor):
    in_shape = np.shape(in_tensor)
    out_tensor = np.zeros(in_shape)

    for i in range(in_shape[1]):
        if(in_tensor[0][i] >0):
            out_tensor[0][i] = in_tensor[0][i]

    return out_tensor

def mat_mul(in_tensor, wt_tensor, bias_tensor):
    in_shape = np.shape(in_tensor)
    wt_shape = np.shape(wt_tensor)

    out_tensor = np.zeros((in_shape[0], wt_shape[0]))

    for i in range(wt_shape[0]):
        out_tensor[0][i] = bias_tensor[i]

        for j in range(in_shape[1]):
            out_tensor[0][i] += in_tensor[0][j] * wt_tensor[i][j]

    return out_tensor

def relu_fc_grad(in_grad_tensor, in_relu_tensor):
    in_shape = np.shape(in_relu_tensor)
    grad_shape = np.shape(in_grad_tensor)

    out_grad = np.zeros(grad_shape)

    for i in range(in_shape[1]):
        for j in range(in_shape[0]):
            if(in_relu_tensor[0][i] > 0):
                out_grad[j][i] = in_grad_tensor[j][i]

    return out_grad

def fc_grad(in_grad_tensor, wt_tensor):
    in_grad_shape = np.shape(in_grad_tensor)    # 10*128
    wt_shape = np.shape(wt_tensor)              # 128*4096

    out_grad = np.zeros((in_grad_shape[0], wt_shape[1]))

    for i in range(in_grad_shape[0]):
        for j in range(wt_shape[1]):
            for k in range(wt_shape[0]):
                out_grad[i][j] += in_grad_tensor[i][k] * wt_tensor[k][j]

    return out_grad

def reshape(in_tensor, shape):
    out_tensor = np.zeros((10,shape[1],shape[2],shape[3]))

    for i in range(10):
        for ic in range(shape[1]):
            for ih in range(shape[2]):
                for iw in range(shape[3]):
                    out_tensor[i][ic][ih][iw] = in_tensor[i][shape[1]*ic + shape[2]*ih + iw]

    return out_tensor

def upsample(in_tensor, activations):
    in_shape = np.shape(in_tensor)
    out_grad = np.zeros((10, in_shape[1], in_shape[2]*2, in_shape[3]*2))

    
    for ic in range(in_shape[1]):
        for ih in range(0,in_shape[2]*2,2):
            for iw in range(0, in_shape[3]*2,2):
                val = [ activations[0][ic][ih][iw],
                        activations[0][ic][ih+1][iw],
                        activations[0][ic][ih][iw+1],
                        activations[0][ic][ih+1][iw+1]]
                
                index = np.argmax(val)

                for i in range(10):
                    if(index == 0):
                        out_grad[i][ic][ih][iw] = in_tensor[i][ic][int(ih/2)][int(iw/2)]

                    if(index == 1):
                        out_grad[i][ic][ih+1][iw] = in_tensor[i][ic][int(ih/2)][int(iw/2)]

                    if(index == 2):
                        out_grad[i][ic][ih][iw+1] = in_tensor[i][ic][int(ih/2)][int(iw/2)]

                    if(index == 3):
                        out_grad[i][ic][ih+1][iw+1] = in_tensor[i][ic][int(ih/2)][int(iw/2)]

    return out_grad

def flipped_conv(in_grad, wt_tensor):
    in_shape = np.shape(in_grad)    # (10,64,16,16) 
    wt_shape = np.shape(wt_tensor)   # (64,64,3,3)

    out_tensor = np.zeros((in_shape[0],wt_shape[1],in_shape[2],in_shape[3]))

    for i in range(in_shape[0]):
        for oc in range(wt_shape[1]):
            for oh in range(in_shape[2]):
                for ow in range(in_shape[3]):
                    for ic in range(in_shape[1]):
                        for kh in range(wt_shape[2]):
                            for kw in range(wt_shape[3]):
                                if(oh+kh-1 <0 or ow+kw-1 <0 or oh+kh > in_shape[2] or ow+kw > in_shape[3]):
                                    current_ip = 0
                                else:
                                    current_ip = in_grad[0][ic][oh+kh-1][ow+kw-1]

                                if(ic == 0 and kh==0 and kw==0 ):
                                    out_tensor[i][oc][oh][ow] = current_ip * wt_tensor[ic][oc][2-kh][2-kw] 
                                else:
                                    out_tensor[i][oc][oh][ow] += current_ip * wt_tensor[ic][oc][2-kh][2-kw]

    return out_tensor


# -----------------------------FP--------------------------------------------------

# print("Conv1")
# out1 = conv_operation(inp, b['conv_layer1.weight'], b['conv_layer1.bias'])
# print("Conv2")
# out2 = conv_operation(out1,b['conv_layer2.weight'], b['conv_layer2.bias'])
# print("MaxPool1")
# out3 = max_pool(out2)
# print("Conv3")
# out4 = conv_operation(out3,b['conv_layer3.weight'], b['conv_layer3.bias'])
# print("Conv4")
# out5 = conv_operation(out4,b['conv_layer4.weight'], b['conv_layer4.bias'])
# print("MaxPool2")
# out6 = max_pool(out5)
# print("FC1")
# out7 = conv_flatten_fc(out6,b['fc1.weight'], b['fc1.bias'])
# print("ReLU1")
# out8 = relu_fc(out7)
# print("FC2")
# out9 = mat_mul(out8,b['fc2.weight'], b['fc2.bias'])

# print(out9)
# print(features['fc2'])

# ----------------------------BP---------------------------------------------------

# print("Grad 1 - fc2")
# grad1 = b['fc2.weight'].cpu().detach().numpy()
# print("Grad 2 - relu")
# grad2 = relu_fc_grad(grad1, out7)
# print("Grad 3 - fc1")
# grad3 = fc_grad(grad2, b['fc1.weight'])
# print("Grad 4 - conv_flatten")
# grad4 = reshape(grad3,np.shape(features['max_pool2']))
# print("Grad 5 - maxpool2")
# grad5 = upsample(grad4, out5)
# print("Grad 6 - conv_layer4")
# grad6 = flipped_conv(grad5,b['conv_layer4.weight'])
# print("Grad 7 - conv_layer3")
# grad7 = flipped_conv(grad6,b['conv_layer3.weight'])
# print("Grad 8 - maxpool1")
# grad8 = upsample(grad7, out2)
# print("Grad 9 - conv_layer2")
# grad9 = flipped_conv(grad8,b['conv_layer2.weight'])
# print("Grad 10 - conv_layer1")
# grad10 = flipped_conv(grad9,b['conv_layer1.weight'])

# -------------------------------------------------------------------------------

write_layer_params_to_file = True

if(write_layer_params_to_file):
    for i in range(len(layers)):
        layer_name = layers[i]
        
        if('max' in layer_name or 'relu' in layer_name):
            continue

        filename = "bin/" +  layer_name + "_weights.bin"
        param_name = layer_name +".weight"
        param = model.state_dict()[param_name].detach().numpy()
        with open(filename, "wb") as f:
            param.tofile(f)
        
        print("Param " + param_name + " printed to file " + filename)

        filename = "bin/" +  layer_name + "_bias.bin"
        param_name = layer_name +".bias"
        param = model.state_dict()[param_name].detach().numpy()
        with open(filename, "wb") as f:
            param.tofile(f)
        
        print("Param " + param_name + " printed to file " + filename)