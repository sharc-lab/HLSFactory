import torch
import torch.nn as nn

class Model(nn.Module):
    def __init__(self):
        super().__init__()
        
        self.conv1 = nn.Conv1d(1, 32, kernel_size=5, padding='same')   
        self.acc = nn.ReLU()
        self.max1 = nn.MaxPool1d(kernel_size=5, stride=2) 
        self.conv2 = nn.Conv1d(32, 32, kernel_size=5, padding = 'same')
        self.acc = nn.ReLU()
        self.max2 = nn.MaxPool1d(kernel_size=5, stride=2) 
        self.conv3 = nn.Conv1d(32, 32, kernel_size=5, padding = 'same')
        self.acc = nn.ReLU()
        self.max3 = nn.MaxPool1d(kernel_size=5, stride=2) 
        self.conv4 = nn.Conv1d(32, 32, kernel_size=5, padding = 'same')
        self.acc = nn.ReLU()
        self.max4 = nn.MaxPool1d(kernel_size=5, stride=2) 
        self.max5 = nn.MaxPool1d(kernel_size=5, stride=2) 

        self.dense1 = nn.Linear(64,32)
        self.acc = nn.ReLU()
        
        self.dense2 = nn.Linear(32,5)
        self.acc = nn.ReLU()

        
    def forward(self, x):
        #print(x.shape)
        x = self.conv1(x)
        #print(x.shape)
        x = self.acc(x)
        x = self.max1(x)
        x = self.conv2(x)
        x = self.acc(x)
        x = self.max2(x)
        x = self.conv3(x)
        x = self.acc(x)
        x = self.max3(x)
        x = self.conv4(x)
        x = self.acc(x)
        x = self.max4(x)
        x = self.max5(x)

        x = x.view(x.shape[0], -1) #flattens
        x = self.dense1(x)
        x = self.acc(x)
        x = self.dense2(x)
        x = self.acc(x)
        
        return x


