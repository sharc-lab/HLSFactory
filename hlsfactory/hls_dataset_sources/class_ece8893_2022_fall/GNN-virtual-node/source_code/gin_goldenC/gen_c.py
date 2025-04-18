import json

f = open('gin-virtual_ep1_noBN_dim100.weights.dict.json', 'r')
dic = json.load(f)

## generate global weights
for key in dic:
    name = key.replace('.', '_').replace('_noBN', '')
    line = 'float ' + name
    # skip the "num_batches_tracked"
    shape = dic[key]['shape']
    if len(shape) == 0:
        continue
    for d in shape:
        line += '[%d]' % d
    line += ';'
    print(line)


print('\n\n')
## generate external global weights
for key in dic:
    name = key.replace('.', '_').replace('_noBN', '')
    line = 'extern float ' + name
    # skip the "num_batches_tracked"
    shape = dic[key]['shape']
    if len(shape) == 0:
        continue
    for d in shape:
        line += '[%d]' % d
    line += ';'
    print(line)

    
print('\n\n')
## generate weight loading
for key in dic:
    name = key.replace('.', '_').replace('_noBN', '')
    # skip the "num_batches_tracked"
    shape = dic[key]['shape']
    if len(shape) == 0:
        continue
    line = 'fseek(f, %d*sizeof(float), SEEK_SET);\n' % int(dic[key]['offset'])
    line += 'fread(%s, sizeof(float), %d, f);\n' % (name, int(dic[key]['length']))
    print(line)
