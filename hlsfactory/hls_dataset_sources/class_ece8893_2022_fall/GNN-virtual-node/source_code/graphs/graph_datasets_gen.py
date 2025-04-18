import torch
from ogb.graphproppred import PygGraphPropPredDataset
from torch_geometric.data import DataLoader
from tqdm import tqdm
import os
import struct

dataset = PygGraphPropPredDataset(name = "ogbg-molhiv") 

split_idx = dataset.get_idx_split() 
train_loader = DataLoader(dataset[split_idx["train"]], batch_size=32, shuffle=True)
valid_loader = DataLoader(dataset[split_idx["valid"]], batch_size=32, shuffle=False)
test_loader = DataLoader(dataset[split_idx["test"]], batch_size=1, shuffle=False)

try:
    os.mkdir("./graph_info")
    os.mkdir("./graph_bin")
except OSError as error:
    print(error) 

for step, batch in enumerate(tqdm(test_loader, desc="Iteration")):
    num = step + 1
    f = open(f'./graph_info/g{num}_info.txt', 'w')
    f.write(str(batch.num_nodes) + "\n" + str(batch.num_edges) + "\n")
    f.close()
    
    data = list(batch.edge_attr.view(-1).numpy())
    f = open(f'./graph_bin/g{num}_edge_attr.bin', 'wb')
    packed = struct.pack('i'*len(data), *data)
    f.write(packed)
    f.close()

    t = torch.transpose(batch.edge_index, 0, 1)
    data = list(t.reshape(-1).numpy())
    f = open(f'./graph_bin/g{num}_edge_list.bin', 'wb')
    packed = struct.pack('i'*len(data), *data)
    f.write(packed)
    f.close()

    data = list(batch.x.view(-1).numpy())
    f = open(f'./graph_bin/g{num}_node_feature.bin', 'wb')
    packed = struct.pack('i'*len(data), *data)
    f.write(packed)
    f.close()

