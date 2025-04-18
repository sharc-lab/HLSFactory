import torch

# global CIM arguments that can be set by the user
class CIMArgs:
    def __init__(self, inference=False, batch_size=16,
                mem_values=torch.tensor([float('inf'), 10000]), 
                sub_array=[128,128], open_rows=128,
                adc_precision=7, weight_precision=8,
                input_precision=8, conversion_type='PU', vdd=1,
                crossbar_type='resistive',
                v_range=1, v_noise=0,
                output_noise=0, resistance_std=[],
                layer_seed=[], map=False,
                R_parasistic=0, stress_time=0,
                calc_BER=False, dummy_column=False,
                debug=False):
        """
        Initializes the CIM parameters.

        Arguments:
            inference (bool): Whether to perform inference.
            batch_size (int): Batch size for inference.
            mem_values (list): Analog value of memory states [HRS, ..., LRS] (e.g. resistance/capicatance for on and off state).
            sub_array (list): Array size (rows x columns).
            open_rows (int): Number of rows that are open in parallel.
            adc_precision (int): ADC precision.
            conversion_type (str): Current to voltage conversion method (PU or TIA).
            vdd (float): Supply voltage.
            output_noise (float): Standard deviation of output column voltage noise.
            resistance_std (list): Standard deviation of resistance of each memory state.
            layer_seed (list): Random seed for each layer of weights. 1 value for each layer
            R_parasistic (float): Parasitic resistance of the power delivery network.
            stress_time (float): Time to stress the memory before inference.
            calc_BER (bool): Whether to calculate error rate of CIM operation.
        """
        
        self.inference = inference
        self.adc_precision = adc_precision
        self.batch_size = batch_size
        self.mem_values = mem_values
        self.sub_array = sub_array
        self.open_rows = open_rows
        self.conversion_type = conversion_type
        self.vdd = vdd
        self.output_noise = output_noise
        self.resistance_std = resistance_std
        self.R_parasitic = R_parasistic
        self.stress_time = stress_time
        self.calc_BER = calc_BER
        self.crossbar_type = crossbar_type
        self.weight_precision = weight_precision
        self.input_precision = input_precision
        self.layer_seed = layer_seed
        self.map = map
        self.dummy_column = dummy_column
        self.v_range = v_range
        self.v_noise = v_noise
        self.debug = debug


