import torch 
import torch.nn as nn
import torchvision
import torchvision.transforms as transforms

batch_size      = 64
num_classes     = 10
learning_rate   = 0.001  
num_epochs      = 20

device  = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
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

model       = ConvNeuralNet(num_classes)
model.to(device)
criterion   = nn.CrossEntropyLoss()
optimizer   = torch.optim.SGD(model.parameters(), lr=learning_rate, weight_decay=0.005, momentum=0.9)
total_step  = len(train_loader)

for epoch in range(num_epochs):
    for i, (images,labels) in enumerate(train_loader):

        images = images.to(device)
        labels = labels.to(device)

        outputs = model(images)
        loss    = criterion(outputs, labels)

        optimizer.zero_grad()
        loss.backward()
        optimizer.step()

    print('Epoch [{}/{}], Loss: {:.4f}'.format(epoch+1, num_epochs, loss.item()))

print('Finished Training')
PATH = './cifar10_net.pth'
torch.save(model.state_dict(), PATH)

with torch.no_grad():
    correct = 0
    total   = 0

    for images,labels in train_loader:
        images = images.to(device)
        labels = labels.to(device)

        outputs = model(images)
        _,predicted = torch.max(outputs.data,1)
        total += labels.size(0)
        correct += (predicted == labels).sum().item()

    print('Accuracy of network on {} train images: {}%'.format(50000, 100*correct/total))