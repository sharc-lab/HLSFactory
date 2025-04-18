import torch
from torch_geometric.data import DataLoader
import torch.optim as optim
import torch.nn.functional as F
from sklearn.metrics import roc_auc_score, mean_squared_error

from tqdm import tqdm
import argparse
import time
import numpy as np
import json
import struct

import torch.multiprocessing
torch.multiprocessing.set_sharing_strategy('file_system')

### importing OGB
from ogb.graphproppred import PygGraphPropPredDataset, Evaluator
from gnn import GNN, GNN_noBN


def eval_rocauc(y_true, y_pred):
    rocauc_list = []
    for i in range(y_true.shape[1]):
        #AUC is only defined when there is at least one positive data.
        if np.sum(y_true[:,i] == 1) > 0 and np.sum(y_true[:,i] == 0) > 0:
            # ignore nan values
            is_labeled = y_true[:,i] == y_true[:,i]
            rocauc_list.append(roc_auc_score(y_true[is_labeled,i], y_pred[is_labeled,i]))


def eval(model, device, loader, evaluator):
    model.eval()
    y_true = []
    y_pred = []
    for step, batch in enumerate(loader):
        batch = batch.to(device)
        #print(batch)
        with torch.no_grad():
            pred = model(batch)
        y_true.append(batch.y.view(pred.shape).detach().cpu())
        y_pred.append(pred.detach().cpu())

    y_true = torch.cat(y_true, dim = 0).numpy()
    y_pred = torch.cat(y_pred, dim = 0).numpy()
    
    input_dict = {"y_true": y_true, "y_pred": y_pred}
    return evaluator.eval(input_dict), y_true, y_pred


############ Device and test_loader #############
device = torch.device("cuda:0")
dataset = PygGraphPropPredDataset(name = 'ogbg-molhiv')
split_idx = dataset.get_idx_split()
evaluator = Evaluator('ogbg-molhiv')
test_loader = DataLoader(dataset[split_idx["test"]], batch_size=1, shuffle=False, num_workers = 1)


########### Load the pre-trained Virtual node GIN model ###########
print('Load the pretrained GNN model')
model = torch.load('gin-virtual_ep1_dim100.pt')
#model = GNN(gnn_type = 'gin', num_tasks = 1, num_layer = 5, emb_dim = 100, drop_ratio = 0.5, virtual_node = True).to(device)
print('Evaluating...')
auc, ytrue, ypred = eval(model, device, test_loader, evaluator)
print('AUC of the original model:')
print(auc)


########### Collect all the golden outputs from Pytorch ###########
graph_id = 1
all_result = {}
f_txt = open('Pytorch_virtual_node_output_dim100.txt', 'w+')
for yt, yp in zip(ytrue, ypred):
    #print(yt[0], yp[0])
    key = 'g' + str(graph_id)
    all_result[key] = {}
    all_result[key]['true'] = float(yt[0])
    all_result[key]['predict'] = float(yp[0])
    graph_id += 1
    f_txt.write(key + ': ' + str(yp[0]) + '\n')
f_txt.close()

f_json = open('virtual_node_all_results_dim100.json', 'w+')
json.dump(all_result, f_json)
f_json.close()

ypred_BN = ypred

############ This part removes the BatchNorm inside MLP (For Virtual Node MLP as well)###############

print('Removing the BatchNorm in the model')

model_noBN = GNN_noBN(gnn_type = 'gin', num_tasks = 1, num_layer = 5, emb_dim = 100, drop_ratio = 0.5, virtual_node = True).to(device)

model_noBN.training=False
model_noBN.eval()
model.training = False
bn_eps = 0.00001
model_noBN.state_dict()['gnn_node_noBN.atom_encoder.atom_embedding_list.0.weight'].copy_( model.state_dict()['gnn_node.atom_encoder.atom_embedding_list.0.weight'] )
model_noBN.state_dict()['gnn_node_noBN.atom_encoder.atom_embedding_list.1.weight'].copy_( model.state_dict()['gnn_node.atom_encoder.atom_embedding_list.1.weight'] )
model_noBN.state_dict()['gnn_node_noBN.atom_encoder.atom_embedding_list.2.weight'].copy_( model.state_dict()['gnn_node.atom_encoder.atom_embedding_list.2.weight'] )
model_noBN.state_dict()['gnn_node_noBN.atom_encoder.atom_embedding_list.3.weight'].copy_( model.state_dict()['gnn_node.atom_encoder.atom_embedding_list.3.weight'] )
model_noBN.state_dict()['gnn_node_noBN.atom_encoder.atom_embedding_list.4.weight'].copy_( model.state_dict()['gnn_node.atom_encoder.atom_embedding_list.4.weight'] )
model_noBN.state_dict()['gnn_node_noBN.atom_encoder.atom_embedding_list.5.weight'].copy_( model.state_dict()['gnn_node.atom_encoder.atom_embedding_list.5.weight'] )
model_noBN.state_dict()['gnn_node_noBN.atom_encoder.atom_embedding_list.6.weight'].copy_( model.state_dict()['gnn_node.atom_encoder.atom_embedding_list.6.weight'] )
model_noBN.state_dict()['gnn_node_noBN.atom_encoder.atom_embedding_list.7.weight'].copy_( model.state_dict()['gnn_node.atom_encoder.atom_embedding_list.7.weight'] )
model_noBN.state_dict()['gnn_node_noBN.atom_encoder.atom_embedding_list.8.weight'].copy_( model.state_dict()['gnn_node.atom_encoder.atom_embedding_list.8.weight'] )

#Load the weight corresponding to initial virtual node embedding
model_noBN.state_dict()['gnn_node_noBN.virtualnode_embedding.weight'].copy_( model.state_dict()['gnn_node.virtualnode_embedding.weight'] )

model_noBN.state_dict()['gnn_node_noBN.convs.0.eps'].copy_( model.state_dict()['gnn_node.convs.0.eps'] )

MLP_weight = model.state_dict()['gnn_node.convs.0.mlp.0.weight']
MLP_bias = model.state_dict()['gnn_node.convs.0.mlp.0.bias']
running_mean = model.state_dict()['gnn_node.convs.0.mlp.1.running_mean']
running_var = model.state_dict()['gnn_node.convs.0.mlp.1.running_var']
bn_weight = model.state_dict()['gnn_node.convs.0.mlp.1.weight']
bn_bias = model.state_dict()['gnn_node.convs.0.mlp.1.bias']

MLP_weight = MLP_weight.t()
MLP_weight = (torch.div(MLP_weight, torch.sqrt(running_var + bn_eps)) * bn_weight).t()
MLP_bias = torch.div((MLP_bias - running_mean), torch.sqrt(running_var + bn_eps)) * bn_weight + bn_bias

model_noBN.state_dict()['gnn_node_noBN.convs.0.mlp.0.weight'].copy_(MLP_weight)
model_noBN.state_dict()['gnn_node_noBN.convs.0.mlp.0.bias'].copy_(MLP_bias)

MLP_weight = model.state_dict()['gnn_node.convs.0.mlp.3.weight']
MLP_bias = model.state_dict()['gnn_node.convs.0.mlp.3.bias']
running_mean = model.state_dict()['gnn_node.batch_norms.0.running_mean']
running_var = model.state_dict()['gnn_node.batch_norms.0.running_var']
bn_weight = model.state_dict()['gnn_node.batch_norms.0.weight']
bn_bias = model.state_dict()['gnn_node.batch_norms.0.bias']

MLP_weight = MLP_weight.t()
MLP_weight = (torch.div(MLP_weight, torch.sqrt(running_var + bn_eps)) * bn_weight).t()
MLP_bias = torch.div((MLP_bias - running_mean), torch.sqrt(running_var + bn_eps)) * bn_weight + bn_bias

model_noBN.state_dict()['gnn_node_noBN.convs.0.mlp.2.weight'].copy_(MLP_weight)
model_noBN.state_dict()['gnn_node_noBN.convs.0.mlp.2.bias'].copy_(MLP_bias)

# Remove batch norm from virtual node MLP
VN_MLP_weight = model.state_dict()['gnn_node.mlp_virtualnode_list.0.0.weight']
VN_MLP_bias = model.state_dict()['gnn_node.mlp_virtualnode_list.0.0.bias']
VN_running_mean = model.state_dict()['gnn_node.mlp_virtualnode_list.0.1.running_mean']
VN_running_var = model.state_dict()['gnn_node.mlp_virtualnode_list.0.1.running_var']
VN_bn_weight = model.state_dict()['gnn_node.mlp_virtualnode_list.0.1.weight']
VN_bn_bias = model.state_dict()['gnn_node.mlp_virtualnode_list.0.1.bias']

VN_MLP_weight =  VN_MLP_weight.t()
VN_MLP_weight = (torch.div(VN_MLP_weight, torch.sqrt(VN_running_var + bn_eps)) * VN_bn_weight).t()
VN_MLP_bias = torch.div((VN_MLP_bias - VN_running_mean), torch.sqrt(VN_running_var + bn_eps)) * VN_bn_weight + VN_bn_bias

model_noBN.state_dict()['gnn_node_noBN.mlp_virtualnode_list.0.0.weight'].copy_(VN_MLP_weight)
model_noBN.state_dict()['gnn_node_noBN.mlp_virtualnode_list.0.0.bias'].copy_(VN_MLP_bias)

VN_MLP_weight = model.state_dict()['gnn_node.mlp_virtualnode_list.0.3.weight']
VN_MLP_bias = model.state_dict()['gnn_node.mlp_virtualnode_list.0.3.bias']
VN_running_mean = model.state_dict()['gnn_node.mlp_virtualnode_list.0.4.running_mean']
VN_running_var = model.state_dict()['gnn_node.mlp_virtualnode_list.0.4.running_var']
VN_bn_weight = model.state_dict()['gnn_node.mlp_virtualnode_list.0.4.weight']
VN_bn_bias = model.state_dict()['gnn_node.mlp_virtualnode_list.0.4.bias']

VN_MLP_weight =  VN_MLP_weight.t()
VN_MLP_weight = (torch.div(VN_MLP_weight, torch.sqrt(VN_running_var + bn_eps)) * VN_bn_weight).t()
VN_MLP_bias = torch.div((VN_MLP_bias - VN_running_mean), torch.sqrt(VN_running_var + bn_eps)) * VN_bn_weight + VN_bn_bias

model_noBN.state_dict()['gnn_node_noBN.mlp_virtualnode_list.0.2.weight'].copy_(VN_MLP_weight)
model_noBN.state_dict()['gnn_node_noBN.mlp_virtualnode_list.0.2.bias'].copy_(VN_MLP_bias)

model_noBN.state_dict()['gnn_node_noBN.convs.0.bond_encoder.bond_embedding_list.0.weight'].copy_(model.state_dict()['gnn_node.convs.0.bond_encoder.bond_embedding_list.0.weight'])
model_noBN.state_dict()['gnn_node_noBN.convs.0.bond_encoder.bond_embedding_list.1.weight'].copy_(model.state_dict()['gnn_node.convs.0.bond_encoder.bond_embedding_list.1.weight'])
model_noBN.state_dict()['gnn_node_noBN.convs.0.bond_encoder.bond_embedding_list.2.weight'].copy_(model.state_dict()['gnn_node.convs.0.bond_encoder.bond_embedding_list.2.weight'])
model_noBN.state_dict()['gnn_node_noBN.convs.1.eps'].copy_( model.state_dict()['gnn_node.convs.1.eps'] )

MLP_weight = model.state_dict()['gnn_node.convs.1.mlp.0.weight']
MLP_bias = model.state_dict()['gnn_node.convs.1.mlp.0.bias']
running_mean = model.state_dict()['gnn_node.convs.1.mlp.1.running_mean']
running_var = model.state_dict()['gnn_node.convs.1.mlp.1.running_var']
bn_weight = model.state_dict()['gnn_node.convs.1.mlp.1.weight']
bn_bias = model.state_dict()['gnn_node.convs.1.mlp.1.bias']

MLP_weight = MLP_weight.t()
MLP_weight = (torch.div(MLP_weight, torch.sqrt(running_var + bn_eps)) * bn_weight).t()
MLP_bias = torch.div((MLP_bias - running_mean), torch.sqrt(running_var + bn_eps)) * bn_weight + bn_bias

model_noBN.state_dict()['gnn_node_noBN.convs.1.mlp.0.weight'].copy_(MLP_weight)
model_noBN.state_dict()['gnn_node_noBN.convs.1.mlp.0.bias'].copy_(MLP_bias)

MLP_weight = model.state_dict()['gnn_node.convs.1.mlp.3.weight']
MLP_bias = model.state_dict()['gnn_node.convs.1.mlp.3.bias']
running_mean = model.state_dict()['gnn_node.batch_norms.1.running_mean']
running_var = model.state_dict()['gnn_node.batch_norms.1.running_var']
bn_weight = model.state_dict()['gnn_node.batch_norms.1.weight']
bn_bias = model.state_dict()['gnn_node.batch_norms.1.bias']

MLP_weight = MLP_weight.t()
MLP_weight = (torch.div(MLP_weight, torch.sqrt(running_var + bn_eps)) * bn_weight).t()
MLP_bias = torch.div((MLP_bias - running_mean), torch.sqrt(running_var + bn_eps)) * bn_weight + bn_bias

model_noBN.state_dict()['gnn_node_noBN.convs.1.mlp.2.weight'].copy_(MLP_weight)
model_noBN.state_dict()['gnn_node_noBN.convs.1.mlp.2.bias'].copy_(MLP_bias)

# Remove batch norm from virtual node MLP
VN_MLP_weight = model.state_dict()['gnn_node.mlp_virtualnode_list.1.0.weight']
VN_MLP_bias = model.state_dict()['gnn_node.mlp_virtualnode_list.1.0.bias']
VN_running_mean = model.state_dict()['gnn_node.mlp_virtualnode_list.1.1.running_mean']
VN_running_var = model.state_dict()['gnn_node.mlp_virtualnode_list.1.1.running_var']
VN_bn_weight = model.state_dict()['gnn_node.mlp_virtualnode_list.1.1.weight']
VN_bn_bias = model.state_dict()['gnn_node.mlp_virtualnode_list.1.1.bias']

VN_MLP_weight =  VN_MLP_weight.t()
VN_MLP_weight = (torch.div(VN_MLP_weight, torch.sqrt(VN_running_var + bn_eps)) * VN_bn_weight).t()
VN_MLP_bias = torch.div((VN_MLP_bias - VN_running_mean), torch.sqrt(VN_running_var + bn_eps)) * VN_bn_weight + VN_bn_bias

model_noBN.state_dict()['gnn_node_noBN.mlp_virtualnode_list.1.0.weight'].copy_(VN_MLP_weight)
model_noBN.state_dict()['gnn_node_noBN.mlp_virtualnode_list.1.0.bias'].copy_(VN_MLP_bias)

VN_MLP_weight = model.state_dict()['gnn_node.mlp_virtualnode_list.1.3.weight']
VN_MLP_bias = model.state_dict()['gnn_node.mlp_virtualnode_list.1.3.bias']
VN_running_mean = model.state_dict()['gnn_node.mlp_virtualnode_list.1.4.running_mean']
VN_running_var = model.state_dict()['gnn_node.mlp_virtualnode_list.1.4.running_var']
VN_bn_weight = model.state_dict()['gnn_node.mlp_virtualnode_list.1.4.weight']
VN_bn_bias = model.state_dict()['gnn_node.mlp_virtualnode_list.1.4.bias']

VN_MLP_weight =  VN_MLP_weight.t()
VN_MLP_weight = (torch.div(VN_MLP_weight, torch.sqrt(VN_running_var + bn_eps)) * VN_bn_weight).t()
VN_MLP_bias = torch.div((VN_MLP_bias - VN_running_mean), torch.sqrt(VN_running_var + bn_eps)) * VN_bn_weight + VN_bn_bias

model_noBN.state_dict()['gnn_node_noBN.mlp_virtualnode_list.1.2.weight'].copy_(VN_MLP_weight)
model_noBN.state_dict()['gnn_node_noBN.mlp_virtualnode_list.1.2.bias'].copy_(VN_MLP_bias)

model_noBN.state_dict()['gnn_node_noBN.convs.1.bond_encoder.bond_embedding_list.0.weight'].copy_( model.state_dict()['gnn_node.convs.1.bond_encoder.bond_embedding_list.0.weight'] )
model_noBN.state_dict()['gnn_node_noBN.convs.1.bond_encoder.bond_embedding_list.1.weight'].copy_( model.state_dict()['gnn_node.convs.1.bond_encoder.bond_embedding_list.1.weight'] )
model_noBN.state_dict()['gnn_node_noBN.convs.1.bond_encoder.bond_embedding_list.2.weight'].copy_( model.state_dict()['gnn_node.convs.1.bond_encoder.bond_embedding_list.2.weight'] )
model_noBN.state_dict()['gnn_node_noBN.convs.2.eps'].copy_( model.state_dict()['gnn_node.convs.2.eps'] )

MLP_weight = model.state_dict()['gnn_node.convs.2.mlp.0.weight']
MLP_bias = model.state_dict()['gnn_node.convs.2.mlp.0.bias']
running_mean = model.state_dict()['gnn_node.convs.2.mlp.1.running_mean']
running_var = model.state_dict()['gnn_node.convs.2.mlp.1.running_var']
bn_weight = model.state_dict()['gnn_node.convs.2.mlp.1.weight']
bn_bias = model.state_dict()['gnn_node.convs.2.mlp.1.bias']

MLP_weight = MLP_weight.t()
MLP_weight = (torch.div(MLP_weight, torch.sqrt(running_var + bn_eps)) * bn_weight).t()
MLP_bias = torch.div((MLP_bias - running_mean), torch.sqrt(running_var + bn_eps)) * bn_weight + bn_bias

model_noBN.state_dict()['gnn_node_noBN.convs.2.mlp.0.weight'].copy_(MLP_weight)
model_noBN.state_dict()['gnn_node_noBN.convs.2.mlp.0.bias'].copy_(MLP_bias)

MLP_weight = model.state_dict()['gnn_node.convs.2.mlp.3.weight']
MLP_bias = model.state_dict()['gnn_node.convs.2.mlp.3.bias']
running_mean = model.state_dict()['gnn_node.batch_norms.2.running_mean']
running_var = model.state_dict()['gnn_node.batch_norms.2.running_var']
bn_weight = model.state_dict()['gnn_node.batch_norms.2.weight']
bn_bias = model.state_dict()['gnn_node.batch_norms.2.bias']

MLP_weight = MLP_weight.t()
MLP_weight = (torch.div(MLP_weight, torch.sqrt(running_var + bn_eps)) * bn_weight).t()
MLP_bias = torch.div((MLP_bias - running_mean), torch.sqrt(running_var + bn_eps)) * bn_weight + bn_bias

model_noBN.state_dict()['gnn_node_noBN.convs.2.mlp.2.weight'].copy_(MLP_weight)
model_noBN.state_dict()['gnn_node_noBN.convs.2.mlp.2.bias'].copy_(MLP_bias)

# Remove batch norm from virtual node MLP
VN_MLP_weight = model.state_dict()['gnn_node.mlp_virtualnode_list.2.0.weight']
VN_MLP_bias = model.state_dict()['gnn_node.mlp_virtualnode_list.2.0.bias']
VN_running_mean = model.state_dict()['gnn_node.mlp_virtualnode_list.2.1.running_mean']
VN_running_var = model.state_dict()['gnn_node.mlp_virtualnode_list.2.1.running_var']
VN_bn_weight = model.state_dict()['gnn_node.mlp_virtualnode_list.2.1.weight']
VN_bn_bias = model.state_dict()['gnn_node.mlp_virtualnode_list.2.1.bias']

VN_MLP_weight =  VN_MLP_weight.t()
VN_MLP_weight = (torch.div(VN_MLP_weight, torch.sqrt(VN_running_var + bn_eps)) * VN_bn_weight).t()
VN_MLP_bias = torch.div((VN_MLP_bias - VN_running_mean), torch.sqrt(VN_running_var + bn_eps)) * VN_bn_weight + VN_bn_bias

model_noBN.state_dict()['gnn_node_noBN.mlp_virtualnode_list.2.0.weight'].copy_(VN_MLP_weight)
model_noBN.state_dict()['gnn_node_noBN.mlp_virtualnode_list.2.0.bias'].copy_(VN_MLP_bias)

VN_MLP_weight = model.state_dict()['gnn_node.mlp_virtualnode_list.2.3.weight']
VN_MLP_bias = model.state_dict()['gnn_node.mlp_virtualnode_list.2.3.bias']
VN_running_mean = model.state_dict()['gnn_node.mlp_virtualnode_list.2.4.running_mean']
VN_running_var = model.state_dict()['gnn_node.mlp_virtualnode_list.2.4.running_var']
VN_bn_weight = model.state_dict()['gnn_node.mlp_virtualnode_list.2.4.weight']
VN_bn_bias = model.state_dict()['gnn_node.mlp_virtualnode_list.2.4.bias']

VN_MLP_weight =  VN_MLP_weight.t()
VN_MLP_weight = (torch.div(VN_MLP_weight, torch.sqrt(VN_running_var + bn_eps)) * VN_bn_weight).t()
VN_MLP_bias = torch.div((VN_MLP_bias - VN_running_mean), torch.sqrt(VN_running_var + bn_eps)) * VN_bn_weight + VN_bn_bias

model_noBN.state_dict()['gnn_node_noBN.mlp_virtualnode_list.2.2.weight'].copy_(VN_MLP_weight)
model_noBN.state_dict()['gnn_node_noBN.mlp_virtualnode_list.2.2.bias'].copy_(VN_MLP_bias)

model_noBN.state_dict()['gnn_node_noBN.convs.2.bond_encoder.bond_embedding_list.0.weight'].copy_( model.state_dict()['gnn_node.convs.2.bond_encoder.bond_embedding_list.0.weight'] )
model_noBN.state_dict()['gnn_node_noBN.convs.2.bond_encoder.bond_embedding_list.1.weight'].copy_( model.state_dict()['gnn_node.convs.2.bond_encoder.bond_embedding_list.1.weight'] )
model_noBN.state_dict()['gnn_node_noBN.convs.2.bond_encoder.bond_embedding_list.2.weight'].copy_( model.state_dict()['gnn_node.convs.2.bond_encoder.bond_embedding_list.2.weight'] )
model_noBN.state_dict()['gnn_node_noBN.convs.3.eps'].copy_( model.state_dict()['gnn_node.convs.3.eps'] )


MLP_weight = model.state_dict()['gnn_node.convs.3.mlp.0.weight']
MLP_bias = model.state_dict()['gnn_node.convs.3.mlp.0.bias']
running_mean = model.state_dict()['gnn_node.convs.3.mlp.1.running_mean']
running_var = model.state_dict()['gnn_node.convs.3.mlp.1.running_var']
bn_weight = model.state_dict()['gnn_node.convs.3.mlp.1.weight']
bn_bias = model.state_dict()['gnn_node.convs.3.mlp.1.bias']

MLP_weight = MLP_weight.t()
MLP_weight = (torch.div(MLP_weight, torch.sqrt(running_var + bn_eps)) * bn_weight).t()
MLP_bias = torch.div((MLP_bias - running_mean), torch.sqrt(running_var + bn_eps)) * bn_weight + bn_bias

model_noBN.state_dict()['gnn_node_noBN.convs.3.mlp.0.weight'].copy_(MLP_weight)
model_noBN.state_dict()['gnn_node_noBN.convs.3.mlp.0.bias'].copy_(MLP_bias)

MLP_weight = model.state_dict()['gnn_node.convs.3.mlp.3.weight']
MLP_bias = model.state_dict()['gnn_node.convs.3.mlp.3.bias']
running_mean = model.state_dict()['gnn_node.batch_norms.3.running_mean']
running_var = model.state_dict()['gnn_node.batch_norms.3.running_var']
bn_weight = model.state_dict()['gnn_node.batch_norms.3.weight']
bn_bias = model.state_dict()['gnn_node.batch_norms.3.bias']

MLP_weight = MLP_weight.t()
MLP_weight = (torch.div(MLP_weight, torch.sqrt(running_var + bn_eps)) * bn_weight).t()
MLP_bias = torch.div((MLP_bias - running_mean), torch.sqrt(running_var + bn_eps)) * bn_weight + bn_bias

model_noBN.state_dict()['gnn_node_noBN.convs.3.mlp.2.weight'].copy_(MLP_weight)
model_noBN.state_dict()['gnn_node_noBN.convs.3.mlp.2.bias'].copy_(MLP_bias)

# Remove batch norm from virtual node MLP
VN_MLP_weight = model.state_dict()['gnn_node.mlp_virtualnode_list.3.0.weight']
VN_MLP_bias = model.state_dict()['gnn_node.mlp_virtualnode_list.3.0.bias']
VN_running_mean = model.state_dict()['gnn_node.mlp_virtualnode_list.3.1.running_mean']
VN_running_var = model.state_dict()['gnn_node.mlp_virtualnode_list.3.1.running_var']
VN_bn_weight = model.state_dict()['gnn_node.mlp_virtualnode_list.3.1.weight']
VN_bn_bias = model.state_dict()['gnn_node.mlp_virtualnode_list.3.1.bias']

VN_MLP_weight =  VN_MLP_weight.t()
VN_MLP_weight = (torch.div(VN_MLP_weight, torch.sqrt(VN_running_var + bn_eps)) * VN_bn_weight).t()
VN_MLP_bias = torch.div((VN_MLP_bias - VN_running_mean), torch.sqrt(VN_running_var + bn_eps)) * VN_bn_weight + VN_bn_bias

model_noBN.state_dict()['gnn_node_noBN.mlp_virtualnode_list.3.0.weight'].copy_(VN_MLP_weight)
model_noBN.state_dict()['gnn_node_noBN.mlp_virtualnode_list.3.0.bias'].copy_(VN_MLP_bias)

VN_MLP_weight = model.state_dict()['gnn_node.mlp_virtualnode_list.3.3.weight']
VN_MLP_bias = model.state_dict()['gnn_node.mlp_virtualnode_list.3.3.bias']
VN_running_mean = model.state_dict()['gnn_node.mlp_virtualnode_list.3.4.running_mean']
VN_running_var = model.state_dict()['gnn_node.mlp_virtualnode_list.3.4.running_var']
VN_bn_weight = model.state_dict()['gnn_node.mlp_virtualnode_list.3.4.weight']
VN_bn_bias = model.state_dict()['gnn_node.mlp_virtualnode_list.3.4.bias']

VN_MLP_weight =  VN_MLP_weight.t()
VN_MLP_weight = (torch.div(VN_MLP_weight, torch.sqrt(VN_running_var + bn_eps)) * VN_bn_weight).t()
VN_MLP_bias = torch.div((VN_MLP_bias - VN_running_mean), torch.sqrt(VN_running_var + bn_eps)) * VN_bn_weight + VN_bn_bias

model_noBN.state_dict()['gnn_node_noBN.mlp_virtualnode_list.3.2.weight'].copy_(VN_MLP_weight)
model_noBN.state_dict()['gnn_node_noBN.mlp_virtualnode_list.3.2.bias'].copy_(VN_MLP_bias)

model_noBN.state_dict()['gnn_node_noBN.convs.3.bond_encoder.bond_embedding_list.0.weight'].copy_( model.state_dict()['gnn_node.convs.3.bond_encoder.bond_embedding_list.0.weight'] )
model_noBN.state_dict()['gnn_node_noBN.convs.3.bond_encoder.bond_embedding_list.1.weight'].copy_( model.state_dict()['gnn_node.convs.3.bond_encoder.bond_embedding_list.1.weight'] )
model_noBN.state_dict()['gnn_node_noBN.convs.3.bond_encoder.bond_embedding_list.2.weight'].copy_( model.state_dict()['gnn_node.convs.3.bond_encoder.bond_embedding_list.2.weight'] )
model_noBN.state_dict()['gnn_node_noBN.convs.4.eps'].copy_( model.state_dict()['gnn_node.convs.4.eps'] )

MLP_weight = model.state_dict()['gnn_node.convs.4.mlp.0.weight']
MLP_bias = model.state_dict()['gnn_node.convs.4.mlp.0.bias']
running_mean = model.state_dict()['gnn_node.convs.4.mlp.1.running_mean']
running_var = model.state_dict()['gnn_node.convs.4.mlp.1.running_var']
bn_weight = model.state_dict()['gnn_node.convs.4.mlp.1.weight']
bn_bias = model.state_dict()['gnn_node.convs.4.mlp.1.bias']

MLP_weight = MLP_weight.t()
MLP_weight = (torch.div(MLP_weight, torch.sqrt(running_var + bn_eps)) * bn_weight).t()
MLP_bias = torch.div((MLP_bias - running_mean), torch.sqrt(running_var + bn_eps)) * bn_weight + bn_bias

model_noBN.state_dict()['gnn_node_noBN.convs.4.mlp.0.weight'].copy_(MLP_weight)
model_noBN.state_dict()['gnn_node_noBN.convs.4.mlp.0.bias'].copy_(MLP_bias)

MLP_weight = model.state_dict()['gnn_node.convs.4.mlp.3.weight']
MLP_bias = model.state_dict()['gnn_node.convs.4.mlp.3.bias']
running_mean = model.state_dict()['gnn_node.batch_norms.4.running_mean']
running_var = model.state_dict()['gnn_node.batch_norms.4.running_var']
bn_weight = model.state_dict()['gnn_node.batch_norms.4.weight']
bn_bias = model.state_dict()['gnn_node.batch_norms.4.bias']

MLP_weight = MLP_weight.t()
MLP_weight = (torch.div(MLP_weight, torch.sqrt(running_var + bn_eps)) * bn_weight).t()
MLP_bias = torch.div((MLP_bias - running_mean), torch.sqrt(running_var + bn_eps)) * bn_weight + bn_bias

model_noBN.state_dict()['gnn_node_noBN.convs.4.mlp.2.weight'].copy_(MLP_weight)
model_noBN.state_dict()['gnn_node_noBN.convs.4.mlp.2.bias'].copy_(MLP_bias)

model_noBN.state_dict()['gnn_node_noBN.convs.4.bond_encoder.bond_embedding_list.0.weight'].copy_( model.state_dict()['gnn_node.convs.4.bond_encoder.bond_embedding_list.0.weight'] )
model_noBN.state_dict()['gnn_node_noBN.convs.4.bond_encoder.bond_embedding_list.1.weight'].copy_( model.state_dict()['gnn_node.convs.4.bond_encoder.bond_embedding_list.1.weight'] )
model_noBN.state_dict()['gnn_node_noBN.convs.4.bond_encoder.bond_embedding_list.2.weight'].copy_( model.state_dict()['gnn_node.convs.4.bond_encoder.bond_embedding_list.2.weight'] )
model_noBN.state_dict()['graph_pred_linear.weight'].copy_( model.state_dict()['graph_pred_linear.weight'] )
model_noBN.state_dict()['graph_pred_linear.bias'].copy_( model.state_dict()['graph_pred_linear.bias'] )


print('Evaluating...')
auc, ytrue, ypred = eval(model_noBN, device, test_loader, evaluator)
print("AUC of the model with no BatchNorm:")
print(auc)

torch.save(model_noBN, 'gin-virtual_ep1_noBN_dim100.pt')

########### Collect all the golden outputs from Pytorch ###########
graph_id = 1
all_result_noBN = {}
f_txt = open('Pytorch_virtual_node_noBN_output_dim100.txt', 'w+')
for yt, yp in zip(ytrue, ypred):
    #print(yt[0], yp[0])
    key = 'g' + str(graph_id)
    all_result_noBN[key] = {}
    all_result_noBN[key]['true'] = float(yt[0])
    all_result_noBN[key]['predict'] = float(yp[0])
    graph_id += 1
    f_txt.write(key + ': ' + str(yp[0]) + '\n')
f_txt.close()

f_json = open('noBN_virtual_node_all_results_dim100.json', 'w+')
json.dump(all_result_noBN, f_json)
f_json.close()

ypred_noBN = ypred

############## Compute the MSE between BN and no BN model predictions ##############
print(ypred_BN.shape)
print(ypred_BN)

print(ypred_noBN.shape)
print(ypred_noBN)
mse = mean_squared_error(ypred_BN, ypred_noBN)
print("MSE (predictions of model with and without BN: )", mse)

#MSE = np.square(np.subtract(ypred_BN, ypred_noBN)).mean()
#print(MSE)



############# Collect all the weights without BatchNorm for golden C ##############

print("collecting weights for the golden C")

weights_dict = {}
weights_data = []
offset = 0
for key in model_noBN.state_dict():
    print(key)
    #print(model_noBN.state_dict()[key].shape)
    #print(model_noBN.state_dict()[key].view(-1).numpy().shape)

    weights_dict[key] = {}
    weights_dict[key]['shape'] = list(model_noBN.state_dict()[key].shape)
    weights_dict[key]['key'] = key
    weights_dict[key]['offset'] = offset
    data = list(model_noBN.state_dict()[key].cpu().view(-1).numpy())
    data_length = len(data)
    weights_dict[key]['length'] = data_length
    offset += data_length
    weights_data += data

f = open('gin-virtual_ep1_noBN_dim100.weights.dict.json', 'w')
json.dump(weights_dict, f)
f.close()

f = open('gin-virtual_ep1_noBN_dim100.weights.all.bin', 'wb')
packed = struct.pack('f'*len(weights_data), *weights_data)
f.write(packed)
f.close()




########### collecting weights for the accelerator #######################

print('Collecting and reorganizing the weights for the accelerator')

model = torch.load('gin-virtual_ep1_noBN_dim100.pt')

### pack MLP weights
MLP_weights_0_1 = model.state_dict()['gnn_node_noBN.convs.0.mlp.0.weight'].cpu().numpy()
MLP_weights_1_1 = model.state_dict()['gnn_node_noBN.convs.1.mlp.0.weight'].cpu().numpy()
MLP_weights_2_1 = model.state_dict()['gnn_node_noBN.convs.2.mlp.0.weight'].cpu().numpy()
MLP_weights_3_1 = model.state_dict()['gnn_node_noBN.convs.3.mlp.0.weight'].cpu().numpy()
MLP_weights_4_1 = model.state_dict()['gnn_node_noBN.convs.4.mlp.0.weight'].cpu().numpy()
MLP_1_weights_all = torch.tensor([MLP_weights_0_1, MLP_weights_1_1, MLP_weights_2_1, MLP_weights_3_1, MLP_weights_4_1])
#print(MLP_1_weights_all.shape)

MLP_weights_0_2 = model.state_dict()['gnn_node_noBN.convs.0.mlp.2.weight'].cpu().numpy()
MLP_weights_1_2 = model.state_dict()['gnn_node_noBN.convs.1.mlp.2.weight'].cpu().numpy()
MLP_weights_2_2 = model.state_dict()['gnn_node_noBN.convs.2.mlp.2.weight'].cpu().numpy()
MLP_weights_3_2 = model.state_dict()['gnn_node_noBN.convs.3.mlp.2.weight'].cpu().numpy()
MLP_weights_4_2 = model.state_dict()['gnn_node_noBN.convs.4.mlp.2.weight'].cpu().numpy()
MLP_2_weights_all = torch.tensor([MLP_weights_0_2, MLP_weights_1_2, MLP_weights_2_2, MLP_weights_3_2, MLP_weights_4_2])
#print(MLP_2_weights_all.shape)

MLP_bias_0_1 = model.state_dict()['gnn_node_noBN.convs.0.mlp.0.bias'].cpu().numpy()
MLP_bias_1_1 = model.state_dict()['gnn_node_noBN.convs.1.mlp.0.bias'].cpu().numpy()
MLP_bias_2_1 = model.state_dict()['gnn_node_noBN.convs.2.mlp.0.bias'].cpu().numpy()
MLP_bias_3_1 = model.state_dict()['gnn_node_noBN.convs.3.mlp.0.bias'].cpu().numpy()
MLP_bias_4_1 = model.state_dict()['gnn_node_noBN.convs.4.mlp.0.bias'].cpu().numpy()
MLP_1_bias_all = torch.tensor([MLP_bias_0_1, MLP_bias_1_1, MLP_bias_2_1, MLP_bias_3_1, MLP_bias_4_1])
#print(MLP_1_bias_all.shape)

MLP_bias_0_2 = model.state_dict()['gnn_node_noBN.convs.0.mlp.2.bias'].cpu().numpy()
MLP_bias_1_2 = model.state_dict()['gnn_node_noBN.convs.1.mlp.2.bias'].cpu().numpy()
MLP_bias_2_2 = model.state_dict()['gnn_node_noBN.convs.2.mlp.2.bias'].cpu().numpy()
MLP_bias_3_2 = model.state_dict()['gnn_node_noBN.convs.3.mlp.2.bias'].cpu().numpy()
MLP_bias_4_2 = model.state_dict()['gnn_node_noBN.convs.4.mlp.2.bias'].cpu().numpy()
MLP_2_bias_all = torch.tensor([MLP_bias_0_2, MLP_bias_1_2, MLP_bias_2_2, MLP_bias_3_2, MLP_bias_4_2])
#print(MLP_2_bias_all.shape)

data = list(MLP_1_weights_all.cpu().view(-1).numpy())
f = open('gin-virtual_ep1_mlp_1_weights_dim100.bin', 'wb')
packed = struct.pack('f'*len(data), *data)
f.write(packed)
f.close()

data = list(MLP_1_bias_all.cpu().view(-1).numpy())
f = open('gin-virtual_ep1_mlp_1_bias_dim100.bin', 'wb')
packed = struct.pack('f'*len(data), *data)
f.write(packed)
f.close()


data = list(MLP_2_weights_all.cpu().view(-1).numpy())
f = open('gin-virtual_ep1_mlp_2_weights_dim100.bin', 'wb')
packed = struct.pack('f'*len(data), *data)
f.write(packed)
f.close()


data = list(MLP_2_bias_all.cpu().view(-1).numpy())
f = open('gin-virtual_ep1_mlp_2_bias_dim100.bin', 'wb')
packed = struct.pack('f'*len(data), *data)
f.write(packed)
f.close()

### pack Virtual Node MLP weights 
VN_MLP_weights_0_0 = model.state_dict()['gnn_node_noBN.mlp_virtualnode_list.0.0.weight'].cpu().numpy()
VN_MLP_weights_1_0 = model.state_dict()['gnn_node_noBN.mlp_virtualnode_list.1.0.weight'].cpu().numpy()
VN_MLP_weights_2_0 = model.state_dict()['gnn_node_noBN.mlp_virtualnode_list.2.0.weight'].cpu().numpy()
VN_MLP_weights_3_0 = model.state_dict()['gnn_node_noBN.mlp_virtualnode_list.3.0.weight'].cpu().numpy()
VN_MLP_0_weights_all = torch.tensor([VN_MLP_weights_0_0, VN_MLP_weights_1_0, VN_MLP_weights_2_0, VN_MLP_weights_3_0])

VN_MLP_weights_0_2 = model.state_dict()['gnn_node_noBN.mlp_virtualnode_list.0.2.weight'].cpu().numpy()
VN_MLP_weights_1_2 = model.state_dict()['gnn_node_noBN.mlp_virtualnode_list.1.2.weight'].cpu().numpy()
VN_MLP_weights_2_2 = model.state_dict()['gnn_node_noBN.mlp_virtualnode_list.2.2.weight'].cpu().numpy()
VN_MLP_weights_3_2 = model.state_dict()['gnn_node_noBN.mlp_virtualnode_list.3.2.weight'].cpu().numpy()
VN_MLP_2_weights_all = torch.tensor([VN_MLP_weights_0_2, VN_MLP_weights_1_2, VN_MLP_weights_2_2, VN_MLP_weights_3_2])

VN_MLP_bias_0_0 = model.state_dict()['gnn_node_noBN.mlp_virtualnode_list.0.0.bias'].cpu().numpy()
VN_MLP_bias_1_0 = model.state_dict()['gnn_node_noBN.mlp_virtualnode_list.1.0.bias'].cpu().numpy()
VN_MLP_bias_2_0 = model.state_dict()['gnn_node_noBN.mlp_virtualnode_list.2.0.bias'].cpu().numpy()
VN_MLP_bias_3_0 = model.state_dict()['gnn_node_noBN.mlp_virtualnode_list.3.0.bias'].cpu().numpy()
VN_MLP_0_bias_all = torch.tensor([VN_MLP_bias_0_0, VN_MLP_bias_1_0, VN_MLP_bias_2_0, VN_MLP_bias_3_0])

VN_MLP_bias_0_2 = model.state_dict()['gnn_node_noBN.mlp_virtualnode_list.0.2.bias'].cpu().numpy()
VN_MLP_bias_1_2 = model.state_dict()['gnn_node_noBN.mlp_virtualnode_list.1.2.bias'].cpu().numpy()
VN_MLP_bias_2_2 = model.state_dict()['gnn_node_noBN.mlp_virtualnode_list.2.2.bias'].cpu().numpy()
VN_MLP_bias_3_2 = model.state_dict()['gnn_node_noBN.mlp_virtualnode_list.3.2.bias'].cpu().numpy()
VN_MLP_2_bias_all = torch.tensor([VN_MLP_bias_0_2, VN_MLP_bias_1_2, VN_MLP_bias_2_2, VN_MLP_bias_3_2])

data = list(VN_MLP_0_weights_all.cpu().view(-1).numpy())
f = open('gin-virtual_ep1_virtualnode_mlp_0_weights_dim100.bin', 'wb')
packed = struct.pack('f'*len(data), *data)
f.write(packed)
f.close()

data = list(VN_MLP_0_bias_all.cpu().view(-1).numpy())
f = open('gin-virtual_ep1_virtualnode_mlp_0_bias_dim100.bin', 'wb')
packed = struct.pack('f'*len(data), *data)
f.write(packed)
f.close()


data = list(VN_MLP_2_weights_all.cpu().view(-1).numpy())
f = open('gin-virtual_ep1_virtualnode_mlp_2_weights_dim100.bin', 'wb')
packed = struct.pack('f'*len(data), *data)
f.write(packed)
f.close()


data = list(VN_MLP_2_bias_all.cpu().view(-1).numpy())
f = open('gin-virtual_ep1_virtualnode_mlp_2_bias_dim100.bin', 'wb')
packed = struct.pack('f'*len(data), *data)
f.write(packed)
f.close()


### merge all the embedding tables into one
### Ahhh this is cumbersome
nd_emb_0 = model.state_dict()['gnn_node_noBN.atom_encoder.atom_embedding_list.0.weight']
nd_emb_1 = model.state_dict()['gnn_node_noBN.atom_encoder.atom_embedding_list.1.weight']
nd_emb_2 = model.state_dict()['gnn_node_noBN.atom_encoder.atom_embedding_list.2.weight']
nd_emb_3 = model.state_dict()['gnn_node_noBN.atom_encoder.atom_embedding_list.3.weight']
nd_emb_4 = model.state_dict()['gnn_node_noBN.atom_encoder.atom_embedding_list.4.weight']
nd_emb_5 = model.state_dict()['gnn_node_noBN.atom_encoder.atom_embedding_list.5.weight']
nd_emb_6 = model.state_dict()['gnn_node_noBN.atom_encoder.atom_embedding_list.6.weight']
nd_emb_7 = model.state_dict()['gnn_node_noBN.atom_encoder.atom_embedding_list.7.weight']
nd_emb_8 = model.state_dict()['gnn_node_noBN.atom_encoder.atom_embedding_list.8.weight']

#print(nd_emb_0.shape)
#print(nd_emb_1.shape)
#print(nd_emb_2.shape)
#print(nd_emb_3.shape)
#print(nd_emb_4.shape)
#print(nd_emb_5.shape)
#print(nd_emb_6.shape)
#print(nd_emb_7.shape)
#print(nd_emb_8.shape)

nd_all = torch.cat((nd_emb_0, nd_emb_1, nd_emb_2, nd_emb_3, nd_emb_4, nd_emb_5, nd_emb_6, nd_emb_7, nd_emb_8), dim=0)
#print(nd_all.shape)


data = list(nd_all.cpu().view(-1).numpy())
f = open('gin-virtual_ep1_nd_embed_dim100.bin', 'wb')
packed = struct.pack('f'*len(data), *data)
f.write(packed)
f.close()

# Virtual node embedding weight
virtualnode_emb = model.state_dict()['gnn_node_noBN.virtualnode_embedding.weight']
data = list(virtualnode_emb.cpu().view(-1).numpy())
f = open('gin-virtual_ep1_virtualnode_embed_dim100.bin', 'wb')
packed = struct.pack('f'*len(data), *data)
f.write(packed)
f.close()

ed_emb_0_0 = model.state_dict()['gnn_node_noBN.convs.0.bond_encoder.bond_embedding_list.0.weight']
ed_emb_0_1 = model.state_dict()['gnn_node_noBN.convs.0.bond_encoder.bond_embedding_list.1.weight']
ed_emb_0_2 = model.state_dict()['gnn_node_noBN.convs.0.bond_encoder.bond_embedding_list.2.weight']

#print(ed_emb_0_0.shape)
#print(ed_emb_0_1.shape)
#print(ed_emb_0_2.shape)

ed_emb_1_0 = model.state_dict()['gnn_node_noBN.convs.1.bond_encoder.bond_embedding_list.0.weight']
ed_emb_1_1 = model.state_dict()['gnn_node_noBN.convs.1.bond_encoder.bond_embedding_list.1.weight']
ed_emb_1_2 = model.state_dict()['gnn_node_noBN.convs.1.bond_encoder.bond_embedding_list.2.weight']

ed_emb_2_0 = model.state_dict()['gnn_node_noBN.convs.2.bond_encoder.bond_embedding_list.0.weight']
ed_emb_2_1 = model.state_dict()['gnn_node_noBN.convs.2.bond_encoder.bond_embedding_list.1.weight']
ed_emb_2_2 = model.state_dict()['gnn_node_noBN.convs.2.bond_encoder.bond_embedding_list.2.weight']

ed_emb_3_0 = model.state_dict()['gnn_node_noBN.convs.3.bond_encoder.bond_embedding_list.0.weight'] 
ed_emb_3_1 = model.state_dict()['gnn_node_noBN.convs.3.bond_encoder.bond_embedding_list.1.weight']
ed_emb_3_2 = model.state_dict()['gnn_node_noBN.convs.3.bond_encoder.bond_embedding_list.2.weight']

ed_emb_4_0 = model.state_dict()['gnn_node_noBN.convs.4.bond_encoder.bond_embedding_list.0.weight']
ed_emb_4_1 = model.state_dict()['gnn_node_noBN.convs.4.bond_encoder.bond_embedding_list.1.weight']
ed_emb_4_2 = model.state_dict()['gnn_node_noBN.convs.4.bond_encoder.bond_embedding_list.2.weight']

ed_all = torch.cat((ed_emb_0_0, ed_emb_0_1, ed_emb_0_2, \
                    ed_emb_1_0, ed_emb_1_1, ed_emb_1_2, \
                    ed_emb_2_0, ed_emb_2_1, ed_emb_2_2, \
                    ed_emb_3_0, ed_emb_3_1, ed_emb_3_2, \
                    ed_emb_4_0, ed_emb_4_1, ed_emb_4_2), dim=0)

#print(ed_all.shape)
data = list(ed_all.cpu().view(-1).numpy())
f = open('gin-virtual_ep1_ed_embed_dim100.bin', 'wb')
packed = struct.pack('f'*len(data), *data)
f.write(packed)
f.close()


#for key in model.state_dict():
#    if 'eps' in key:
#        print(model.state_dict()[key].numpy(), key)


eps0 = model.state_dict()['gnn_node_noBN.convs.0.eps']
eps1 = model.state_dict()['gnn_node_noBN.convs.1.eps']
eps2 = model.state_dict()['gnn_node_noBN.convs.2.eps']
eps3 = model.state_dict()['gnn_node_noBN.convs.3.eps']
eps4 = model.state_dict()['gnn_node_noBN.convs.4.eps']
eps_all = torch.tensor([eps0, eps1, eps2, eps3, eps4])
data = list(eps_all.cpu().view(-1).numpy())
f = open('gin-virtual_ep1_eps_dim100.bin', 'wb')
packed = struct.pack('f'*len(data), *data)
f.write(packed)
f.close()



pred_w = model.state_dict()['graph_pred_linear.weight']
pred_b = model.state_dict()['graph_pred_linear.bias']
#print(pred_w.shape, pred_b.shape)

data = list(pred_w.cpu().view(-1).numpy())
f = open('gin-virtual_ep1_pred_weights_dim100.bin', 'wb')
packed = struct.pack('f'*len(data), *data)
f.write(packed)
f.close()


data = list(pred_b.cpu().view(-1).numpy())
f = open('gin-virtual_ep1_pred_bias_dim100.bin', 'wb')
packed = struct.pack('f'*len(data), *data)
f.write(packed)
f.close()

print('All weights are written into *_dim100.bin\n')
