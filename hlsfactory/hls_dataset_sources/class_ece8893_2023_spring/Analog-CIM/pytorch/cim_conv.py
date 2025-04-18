import torch
from torch.nn.functional import unfold
from . import cim_sim

def cim_conv2d(cim_args, quant_input, scale_input, quant_weight, scale_weight, 
               bias=None, stride=(1,1), padding=(0,0), dilation=(1,1), groups=1):
    """
    Performs 2D convolution by converting it into a matrix multiplication where each column of the weight matrix represents a filter.

    Arguments:
        input (torch.Tensor): Input tensor of shape (N, C_in, H_in, W_in).
        weight (torch.Tensor): Weight tensor of shape (C_out, C_in/groups, kH, kW).
        bias (torch.Tensor): Bias tensor of shape (C_out).
        stride (int or tuple): Stride of the convolution.
        padding (int or tuple): Zero-padding added to both sides of the input.
        dilation (int or tuple): Spacing between kernel elements.
        groups (int): Number of blocked connections from input channels to output channels.

    Returns:
        torch.Tensor: Output tensor of shape (N, C_out, H_out, W_out).
    """

    input_shape = quant_input.shape
    weight_shape = quant_weight.shape

    # Unfold the input tensor into a matrix
    quant_input = unfold(quant_input, kernel_size=quant_weight.shape[-2:], dilation=dilation,
                            padding=padding, stride=stride).transpose(1, 2).flatten(start_dim=0, end_dim=1)
    
    # Convert inputs and weights to integers
    quant_input = quant_input * scale_input
    quant_weight = quant_weight * scale_weight

    scale = (scale_input * scale_weight).view(1,-1)

    # Shift inputs and weights so they are positive
    shift_input = -torch.min(quant_input)
    shift_weight = -torch.min(quant_weight)
    quant_input = quant_input + shift_input
    quant_weight = quant_weight + shift_weight
    
    # Add a dummy row to the input matrix
    quant_input = torch.cat((quant_input, torch.full((1,quant_input.shape[1]), fill_value=shift_input, device='cuda')), dim=0)

    # Reshape the weight tensor into a matrix
    quant_weight = quant_weight.view(weight_shape[0], -1).t()

    # Add a dummy column to the weight matrix
    quant_weight = torch.cat((quant_weight, torch.full((quant_weight.shape[0],1), fill_value=shift_weight, device='cuda')), dim=1)

    # output = torch.matmul(quant_input, quant_weight)
    
    # Perform matrix multiplication on CIM crossbar
    quant_input = quant_input.to(torch.int32)     # THIS PART HAS SOME LOSS
    quant_weight = quant_weight.to(torch.int32)   # THIS PART HAS SOME LOSS


    output = cim_sim.simulate_array(cim_args, quant_input, quant_weight)

    # compare outputs
    # diff = torch.abs(output_correct - output)
    # total_loss = torch.sum(diff)
    # print(total_loss) 

    out_dummy_row = output[-1,:-1].unsqueeze(0) # extract dummy row
    out_dummy_col = output[:-1,-1].unsqueeze(1) # extract dummy column
    shift = output[-1,-1]                       # extract dummy element

    output = output[:-1,:-1] # remove dummy row and column   

    # Remove shifts
    output = output - out_dummy_row - out_dummy_col + shift

    # Rescale
    output = output/scale
    
    # Reshape the output tensor
    N = input_shape[0]
    C_out = weight_shape[0]
    H_out = (input_shape[-2] + 2 * padding[0] - dilation[0] * (weight_shape[-2] - 1) - 1) // stride[0] + 1
    W_out = (input_shape[-1] + 2 * padding[1] - dilation[1] * (weight_shape[-1] - 1) - 1) // stride[1] + 1
    output = output.view(N, H_out, W_out, C_out).permute(0, 3, 1, 2)

    # Add bias if provided
    if bias is not None:
        output += bias.view(1, -1, 1, 1)
    
    return output
