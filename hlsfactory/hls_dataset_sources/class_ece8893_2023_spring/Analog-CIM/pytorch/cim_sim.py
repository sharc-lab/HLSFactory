import torch
import numpy as np
import math
from . import cim_mapping

def simulate_array(args, input2d, weight2d):

    assert(input2d.dtype==torch.int32)
    assert(weight2d.dtype==torch.int32)

    # store original weight shape
    weight2d_shape = weight2d.shape

    # convert weights to eNVM cell values
    weight2d = convert_weights(args, weight2d)

    # map weights to sub-arrays
    if args.map == True:
        cim_mapping.map_weights(args, weight2d, weight2d_shape)

    # calculate reference voltages using IR drop
    calc_vref(args)

    ADC_out = torch.zeros((input2d.shape[0], weight2d.shape[1]), device='cuda')
    Psum = torch.zeros_like(ADC_out)

    # divide the weight matrix into partitions
    num_partitions = math.ceil(weight2d.shape[0]/args.open_rows) 
    
    # calculate ADC outputs for each bit of the input
    for i in range(args.input_precision):
        mask = 2**i
        input = (input2d & mask) >> i

        # calculate partial sum for each partition
        Psum[:,:] = 0

        for part in range(num_partitions):  

            start_row = part*args.open_rows
            end_row   = start_row + args.open_rows

            # get digital outputs from the ADCs
            out = ADC_output(args, input[:, start_row:end_row], weight2d[start_row:end_row, :])

            if args.dummy_column == True:
                # the outputs from the offset cancelling dummy column is the last column in the Psum matrix
                dummy_out = Psum[:, -1]

                # cancel off-state currents
                Psum -= dummy_out.unsqueeze(1)

            if args.calc_BER == True:
                Psum_correct = torch.matmul(input[:, start_row:end_row].to(dtype=torch.float32), args.binary_weights[start_row:end_row, :]).to(dtype=torch.int32)
                # np.savetxt('Psum_correct.txt', Psum_correct.cpu().numpy(), fmt='%d')
                # np.savetxt('Psum1.txt', Psum.cpu().numpy(), fmt='%d')
                diff = torch.abs(Psum_correct - out)
                total_loss = torch.sum(diff)
                print(total_loss)            
                # CIM_util.calc_BER(args, ADC_out, ADC_out_correct)

            # add partition output to total output of the sub array
            Psum += out

        # scale partial sum for input bit significance
        Psum *= mask

        # add partition output to total output of the sub array
        ADC_out += Psum

    if args.debug == True:
        out_correct = torch.matmul(input2d.to(dtype=torch.float32), args.binary_weights).to(dtype=torch.int32)
        diff = torch.abs(out_correct - ADC_out)
        total_loss = torch.sum(diff)
        print(total_loss)  

    # create a mask for the weight precision
    max_val = 2**args.weight_precision
    base = len(args.mem_values)
    cols_per_weight = math.ceil(math.log(max_val, base))
    weights_mask = base**torch.arange(cols_per_weight, device='cuda').flip(0)

    # split output into groups of each dot product
    ADC_out = ADC_out.reshape(ADC_out.shape[0], int(ADC_out.shape[1]/cols_per_weight), cols_per_weight)

    # multiply each dot product by the weight mask
    ADC_out *= weights_mask

    # add output bits together and accumulate total output
    output = ADC_out.sum(dim=-1)

    return output 

def ADC_output(args, inputs, weights):
    
    inputs = inputs.to(dtype=torch.float32)

    equiv_cond = torch.matmul(inputs, weights)

    del weights
    del inputs

    # # subtract memory IR drop
    # vdd = args.vdd - args.mem_IR_drop

    vdd = args.vdd

    if args.conversion_type == 'TIA':
        # use trans-impedence amplifier to convert current to voltage
        # BL_voltages = torch.mul(torch.div(vdd, equiv_res), Rf)
        BL_voltages = torch.mul(equiv_cond, vdd*args.Rf)
    elif args.conversion_type == 'I2V': # converting current sum to voltage using pull-up PMOS
        BL_voltages = vdd - equiv_cond*args.res_divider
        # min = torch.min(BL_voltages)
        # if (min < 0):
        #     print(min)
    else:
        # use voltage divider to convert current to voltage
        BL_voltages = torch.div(vdd, 1 + torch.mul(equiv_cond, (args.res_divider + args.Rpdn)))

    if args.v_noise > 0:
        # print("min: ", torch.min(BL_voltages))
        # print("max: ", torch.max(BL_voltages))
        std = torch.normal(mean=0.0, std=args.v_noise, size=BL_voltages.shape, device='cuda')
        # print("min: ", torch.min(std))
        # print("max: ", torch.max(std)) 
        # add to BL voltages depending on std of nosie
        BL_voltages += std
        # print("min: ", torch.min(BL_voltages))
        # print("max: ", torch.max(BL_voltages))

    return sense_voltage(args, BL_voltages)

def convert_weights(args, weights):

    # convert weights to resistances and inject with noise
    n_ary = cim_mapping.convert_to_n_ary(weights, base=len(args.mem_values), bits=args.weight_precision)

    # save binary weights
    if args.calc_BER == True or args.debug == True:
        args.binary_weights = n_ary.clone().to(dtype=torch.float32)

    if len(args.resistance_std) == 0:
        weights = args.mem_values[n_ary]

    else:
        # augment weights with noise
        weights = torch.normal(mean=args.mem_values[n_ary], std=args.resistance_std[n_ary], device='cuda')

    if args.stress_time > 0:
        # augment weights with resistance drift
        drift_mean = calc_drift(args.stress_time)
        drift_std = 1500 # for my own drift tests the std was 1500
        weights += torch.normal(mean=drift_mean, std=drift_std, size=weights.shape, device='cuda')

    # if we are using resistive crossbar, convert to conductance
    if args.crossbar_type == 'resistive':
        return 1/weights
    elif args.crossbar_type == 'capacitive':
        return weights

def calc_drift(stress_time):
    ## TODO: incorporate drift (which direction does each state drift, by how much?)
    ## TODO: allow for user specified drift

    y0 = -55918
    a1 = 26248
    a2 = 12797
    a3 = 13173
    t1 = 10.1545
    t2 = 156.547
    t3 = 156.593

    drift = a1*math.exp(-stress_time/t1) + a2*math.exp(-stress_time/t2) + a3*math.exp(-stress_time/t3) + y0
    
    return drift 

def calc_vref(args):
    ## NOTE: THIS DOES NOT SUPPORT MLC YET

    # calculate voltage references
    num_refs = (2**args.adc_precision)
    x = torch.arange(num_refs, device='cuda') + 1

    # # subtract IR drop for this ADC block
    # vdd = args.vdd - args.logic_IR_drop

    vdd = args.vdd
    LRS = args.mem_values[-1]
    HRS = args.mem_values[0]

    r_max = 1/(x/LRS)
    r_min = 1/((args.open_rows-(x-1))/HRS + (x-1)/LRS)

    if args.conversion_type == 'PU':
        v_max = vdd*(r_max/(args.res_divider + r_max))
        v_min = vdd*(r_min/(args.res_divider + r_min))

    elif args.conversion_type == 'TIA':
        args.Rf = LRS/args.open_rows
        v_max = vdd*(args.Rf/(r_min))
        v_min = vdd*(args.Rf/(r_max))

    ###################################################################
    # v_max = torch.cat((torch.tensor([vdd], device='cuda'), v_max), 0)

    # args.v_ref = (v_min[:-1]+v_max[1:])/2
    args.v_ref = (v_min+v_max)/2
    ###################################################################


def sense_voltage(args, BL_voltages):

    num_refs = 2**args.adc_precision

    if args.conversion_type == 'PU':
        ADC_output = num_refs - torch.bucketize(BL_voltages, args.v_ref.flip(0), out_int32=True, right=True)

    elif args.conversion_type == 'TIA':
        ADC_output = torch.bucketize(BL_voltages, args.v_ref, out_int32=True, right=True)

    return ADC_output