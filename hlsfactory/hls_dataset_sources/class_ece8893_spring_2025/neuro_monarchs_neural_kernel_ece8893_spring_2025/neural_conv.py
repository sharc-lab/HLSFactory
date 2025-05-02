import os
import torch
import torch.nn as nn
import numpy as np
import argparse


def parse_arguments():
    """
    Parse command-line arguments.

    Returns:
        argparse.Namespace: Parsed arguments.
    """
    parser = argparse.ArgumentParser(description="The neural part reference computation.")

    parser.add_argument(
        '--model_path',
        type=str,
        required=True,
        help='Path to the trained model checkpoint (e.g., model_best.pth.tar)'
    )

    parser.add_argument(
        '--input_path',
        type=str,
        required=True,
        help='Path to the input .npy file (e.g., input_1.npy)'
    )

    parser.add_argument(
        '--output_ref_path',
        type=str,
        required=True,
        help='Path to the reference output .npy file (e.g., output_1.npy)'
    )

    parser.add_argument(
        '--device',
        type=str,
        default='cpu',
        help='Computation device to use: "cpu" or "cuda:X" (e.g., "cuda:0")'
    )

    return parser.parse_args()



__all__ = ['ResNet', 'resnet18']

model_urls = {"resnet18": "https://download.pytorch.org/models/resnet18-f37072fd.pth"}


def conv3x3(in_planes, out_planes, stride=1, groups=1, dilation=1):
    """3x3 convolution with padding"""
    return nn.Conv2d(in_planes, out_planes, kernel_size=3, stride=stride,
                     padding=dilation, groups=groups, bias=False, dilation=dilation)

def conv1x1(in_planes, out_planes, stride=1):
    """1x1 convolution"""
    return nn.Conv2d(in_planes, out_planes, kernel_size=1, stride=stride, bias=False)

class Bottleneck(nn.Module):
    # Bottleneck in torchvision places the stride for downsampling at 3x3 convolution(self.conv2)
    # while original implementation places the stride at the first 1x1 convolution(self.conv1)
    # according to "Deep residual learning for image recognition"https://arxiv.org/abs/1512.03385.
    # This variant is also known as ResNet V1.5 and improves accuracy according to
    # https://ngc.nvidia.com/catalog/model-scripts/nvidia:resnet_50_v1_5_for_pytorch.

    expansion: int = 4

    def __init__(self, inplanes: int, planes: int, stride: int = 1, downsample= None,
        groups: int = 1, base_width: int = 64, dilation: int = 1, norm_layer= None) -> None:
        super().__init__()
        if norm_layer is None:
            norm_layer = nn.BatchNorm2d
        width = int(planes * (base_width / 64.0)) * groups
        # Both self.conv2 and self.downsample layers downsample the input when stride != 1
        self.conv1 = conv1x1(inplanes, width)
        self.bn1 = norm_layer(width)
        self.conv2 = conv3x3(width, width, stride, groups, dilation)
        self.bn2 = norm_layer(width)
        self.conv3 = conv1x1(width, planes * self.expansion)
        self.bn3 = norm_layer(planes * self.expansion)
        self.relu = nn.ReLU(inplace=True)
        self.downsample = downsample
        self.stride = stride

    def forward(self, x):
        identity = x

        out = self.conv1(x)
        out = self.bn1(out)
        out = self.relu(out)

        out = self.conv2(out)
        out = self.bn2(out)
        out = self.relu(out)

        out = self.conv3(out)
        out = self.bn3(out)

        if self.downsample is not None:
            identity = self.downsample(x)

        out += identity
        out = self.relu(out)

        return out


class BasicBlock(nn.Module):
    expansion = 1
    def __init__(self, inplanes, planes, stride=1, downsample=None, groups=1,
                 base_width=64, dilation=1, norm_layer=None):
        super(BasicBlock, self).__init__()
        if norm_layer is None:
            norm_layer = nn.BatchNorm2d
        if groups != 1 or base_width != 64:
            raise ValueError('BasicBlock only supports groups=1 and base_width=64')
        if dilation > 1:
            raise NotImplementedError("Dilation > 1 not supported in BasicBlock")
        self.conv1 = conv3x3(inplanes, planes, stride)
        self.bn1 = norm_layer(planes)
        self.relu = nn.ReLU(inplace=True)
        # self.relu = nn.LeakyReLU(0.1,inplace=True)
        self.conv2 = conv3x3(planes, planes)
        self.bn2 = norm_layer(planes)
        self.downsample = downsample
        self.stride = stride

    def forward(self, x):
        identity = x
        out = self.conv1(x)
        out = self.bn1(out)
        out = self.relu(out)
        out = self.conv2(out)
        out = self.bn2(out)
        if self.downsample is not None:
            identity = self.downsample(x)
        out += identity
        out = self.relu(out)
        return out

class ResNet(nn.Module):
    def __init__(self, block, layers, num_classes= 512, zero_init_residual=False,
                 groups=1, width_per_group=64, replace_stride_with_dilation=None,
                 norm_layer=None, pretrained=False, no_maxpool=False):
        super(ResNet, self).__init__()
        if norm_layer is None:
            norm_layer = nn.BatchNorm2d
        self._norm_layer = norm_layer
        self.inplanes = 64
        self.dilation = 1
        self.in_channels = 3 if pretrained else 1
        if replace_stride_with_dilation is None:
            replace_stride_with_dilation = [False, False, False]
        if len(replace_stride_with_dilation) != 3:
            raise ValueError("replace_stride_with_dilation should be None "
                             "or a 3-element tuple, got {}".format(replace_stride_with_dilation))
        self.groups = groups
        self.base_width = width_per_group

        # If no maxpool -> stride 1 conv and Identity maxpool        
        if no_maxpool:
            self.conv1 = nn.Conv2d(self.in_channels, self.inplanes, kernel_size=7, stride=1, padding=3, bias=False)
            self.maxpool = nn.Identity()
        else: 
            self.conv1 = nn.Conv2d(self.in_channels, self.inplanes, kernel_size=7, stride=2, padding=3, bias=False)
            self.maxpool = nn.MaxPool2d(kernel_size=3, stride=2, padding=1)

        self.bn1 = norm_layer(self.inplanes)
        self.relu = nn.ReLU(inplace=True)

        self.layer1 = self._make_layer(block, 64, layers[0])
        self.layer2 = self._make_layer(block, 128, layers[1], stride=2, dilate=replace_stride_with_dilation[0])
        self.layer3 = self._make_layer(block, 256, layers[2], stride=2, dilate=replace_stride_with_dilation[1])
        self.layer4 = self._make_layer(block, 512, layers[3], stride=2, dilate=replace_stride_with_dilation[2])
        self.avgpool = nn.AdaptiveAvgPool2d((1, 1))
        self.Tanh=nn.Tanh()
        self.fc = nn.Linear(512 * block.expansion, num_classes)

        for m in self.modules():
            if isinstance(m, nn.Conv2d):
                nn.init.kaiming_normal_(m.weight, mode='fan_out', nonlinearity='relu')
            elif isinstance(m, (nn.BatchNorm2d, nn.GroupNorm)):
                nn.init.constant_(m.weight, 1)
                nn.init.constant_(m.bias, 0)

        if zero_init_residual:
            for m in self.modules():
                if isinstance(m, BasicBlock):
                    nn.init.constant_(m.bn2.weight, 0)

    def _make_layer(self, block, planes, blocks, stride=1, dilate=False):
        norm_layer = self._norm_layer
        downsample = None
        previous_dilation = self.dilation
        if dilate:
            self.dilation *= stride
            stride = 1
        if stride != 1 or self.inplanes != planes * block.expansion:
            downsample = nn.Sequential(
                conv1x1(self.inplanes, planes * block.expansion, stride),
                norm_layer(planes * block.expansion))

        layers = []
        layers.append(block(self.inplanes, planes, stride, downsample, self.groups,
                            self.base_width, previous_dilation, norm_layer))
        self.inplanes = planes * block.expansion
        for _ in range(1, blocks):
            layers.append(block(self.inplanes, planes, groups=self.groups,
                                base_width=self.base_width, dilation=self.dilation,
                                norm_layer=norm_layer))

        return nn.Sequential(*layers)

    def forward(self, x):
        x = x.repeat(1,self.in_channels,1,1)
        x = self.conv1(x)
        x = self.bn1(x)
        x = self.relu(x)
        x = self.maxpool(x)
        x = self.layer1(x)
        x = self.layer2(x)
        x = self.layer3(x)
        x = self.layer4(x)
        x = self.avgpool(x)
        x = torch.flatten(x, 1)
        x = self.fc(x)
        x = self.Tanh(x)
        return x

def _resnet(arch, block, layers, pretrained, progress,**kwargs):
    model = ResNet(block, layers, pretrained=pretrained,**kwargs)

    if pretrained:
        # load from ImageNet-1k pretrained model
        # This is a advanced version to load only layers with same name and size 
        # (first conv and last fc are not loaded)
        pretrained_state = torch.hub.load_state_dict_from_url(model_urls[arch], progress=progress)
        model_state = model.state_dict()
        pretrained_state = { k:v for k,v in pretrained_state.items() if k in model_state and v.size() == model_state[k].size() }
        model_state.update(pretrained_state)
        model.load_state_dict(model_state)

    return model

def resnet18(pretrained=False, progress: bool=True, **kwargs):
    return _resnet('resnet18', BasicBlock, [2, 2, 2, 2], pretrained, progress,**kwargs)

# Extracted last layer model
class Last_Layer(nn.Module):
    """
    A wrapper model that takes the output from layer3 (i.e., the input to layer4)
    and passes it through layer4 -> avgpool -> flatten -> fc -> Tanh.
    """
    def __init__(self, original_resnet):
        super(Last_Layer, self).__init__()
        # Extract the relevant “sub-layers” from the original model
        self.layer4 = original_resnet.layer4
        self.avgpool = original_resnet.avgpool
        self.fc = original_resnet.fc
        self.tanh = original_resnet.Tanh

    def forward(self, x):
        # x should be what would ordinarily be fed into layer4
        # (i.e., the output from layer3 in the original ResNet).
        x = self.layer4(x)
        x = self.avgpool(x)
        x = torch.flatten(x, 1)
        x = self.fc(x)
        x = self.tanh(x)
        return x



def mse_numpy(array1: np.ndarray, array2: np.ndarray) -> float:
    """
    Calculate the Mean Squared Error (MSE) between two NumPy arrays.

    Parameters:
    - array1 (np.ndarray): First input array.
    - array2 (np.ndarray): Second input array.

    Returns:
    - float: The computed MSE value.

    Raises:
    - ValueError: If the input arrays do not have the same shape.
    """
    if array1.shape != array2.shape:
        raise ValueError(f"Shape mismatch: array1.shape = {array1.shape}, array2.shape = {array2.shape}")
    
    # Compute the squared differences
    squared_diff = (array1 - array2) ** 2
    
    # Compute the mean of the squared differences
    mse = np.mean(squared_diff)
    
    return mse


args = parse_arguments()

# Set up device

device = torch.device(args.device if torch.cuda.is_available() and args.device.startswith("cuda") else "cpu")
if args.device.startswith("cuda") and not torch.cuda.is_available():
    print(f"CUDA is not available. Falling back to CPU.")
    device = torch.device("cpu")

print(f"Using device: {device}")

model = resnet18(pretrained = 1, progress = True, num_classes=512, no_maxpool=1)
model_path = args.model_path
if os.path.isfile(model_path):
    checkpoint = torch.load(model_path)
    model.load_state_dict(checkpoint['state_dict_model'])
model.eval()   

last_layer = Last_Layer(model)
last_layer.to(device) 
last_layer.eval()

last_layer_input = np.load(args.input_path)
last_layer_input_tensor = torch.tensor(last_layer_input, dtype = torch.float32)
last_layer_input_tensor = last_layer_input_tensor.to(device)
last_layer_output_tensor = last_layer(last_layer_input_tensor)
last_layer_output = last_layer_output_tensor.detach().cpu().numpy()


output_ref = np.load(args.output_ref_path)


MSE_loss_last_layer = mse_numpy(last_layer_output, output_ref)
print("the MSE loss is:", MSE_loss_last_layer)


