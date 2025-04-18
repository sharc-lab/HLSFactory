import torch 
import torch.nn as nn
import torchvision
import torchvision.transforms as transforms
from torchsummary import summary

batch_size      = 64
num_classes     = 10
learning_rate   = 0.001  
num_epochs      = 20

device  = torch.device('cuda')
print("Using", device)

all_transforms  = transforms.Compose([  transforms.Resize((32,32)),
                                        transforms.ToTensor(),
                                        transforms.Normalize(mean = [0.4914, 0.4822, 0.4465],
                                                             std  = [0.2023, 0.1994, 0.2010])
                                    ])

train_dataset   = torchvision.datasets.CIFAR10( root = './data',
                                                train = True,
                                                transform = all_transforms,
                                                download = True)

test_dataset    = torchvision.datasets.CIFAR10( root = './data',
                                                train = False,
                                                transform = all_transforms,
                                                download = True)

train_loader    = torch.utils.data.DataLoader(  dataset = train_dataset,
                                                batch_size = batch_size,
                                                shuffle = True)

test_loader     = torch.utils.data.DataLoader(  dataset = test_dataset,
                                                batch_size = batch_size,
                                                shuffle = True)

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
model       = ConvNeuralNet(num_classes)
model.to(device)
model.load_state_dict(torch.load(PATH))
model.eval()
print(model)

a = list(model.parameters())
print(a[0].size())

summary(model, (3,32,32))