#include <torch/torch.h>
#include <tuple>
#include <string>

using namespace std;

class CIM_Args
{
public:
    // opening rows in pytorch increasing sim time
    // limited on/off ratio 16, large array 128x128, can only open a small number of rows (16)
    // resnet first layer would be 147 rows when unrolled, can set subarray size of 147 + open rows can be 147

    bool m_inference;
    int m_batch_size;
    torch::Tensor m_mem_values;
    pair<int, int> m_sub_array;
    int m_open_rows;
    int m_adc_precision;
    int m_weight_precision;
    int m_input_precision;
    float m_vdd;
    torch::Tensor m_res_divider;
    torch::Tensor m_vref;
    float m_num_refs;

    CIM_Args()
    {
        m_inference = false;
        m_batch_size = 16;
        m_mem_values = torch::tensor({148000, 1000});
        m_sub_array.first = 147;
        m_sub_array.second = 520;
        m_open_rows = 147;
        m_adc_precision = 8;
        m_weight_precision = 8;
        m_input_precision = 8;
        m_vdd = 1;
    }

    CIM_Args(bool inference, int batch_size, torch::Tensor mem_values,
            pair<int, int> sub_array, int open_rows, int adc_precision,
            int weight_precision, int input_precision, int vdd)
    {
        m_inference = inference;
        m_batch_size = batch_size;
        m_mem_values = mem_values;
        m_sub_array = sub_array;
        m_open_rows = open_rows;
        m_adc_precision = adc_precision;
        m_weight_precision = weight_precision;
        m_input_precision = input_precision;
        m_vdd = vdd;
    }

    //bool performInference() { return m_inference; }
    //int getBatchSize() { return m_batch_size; }
    //pair<float, float> getMemVals() { return make_pair(m_mem_val_lrs, m_mem_val_hrs); }
    //pair<int, int> getSubarraySize() { return make_pair(m_subarray_rows, m_subarray_cols); }
    //int getOpenRows() { return m_open_rows; }
    //int getADCPrecision() { return m_adc_precision; }
    //int getWeightPrecision() { return m_weight_precision; }
    //int getInputPrecision() { return m_input_precision; }
    //int getVDD() { return m_vdd; }
    //float getOutputNoise() { return m_output_noise; }
    //float getResParasitic() { return m_r_parasitic; }
    //float getStressTime() { return m_stress_time; }
    //bool calcBER() { return m_calc_BER; }
    //
    //void setBatchSize(int batch_size) { m_batch_size = batch_size; }
    //void setMemVals(pair<float, float> mem_vals)
    //{
    //    m_mem_val_lrs = mem_vals.first;
    //    m_mem_val_hrs = mem_vals.second;
    //}
    //void setSubaraySize(pair<int, int> subarray_size)
    //{
    //    m_subarray_rows = subarray_size.first;
    //    m_subarray_cols = subarray_size.second;
    //}  
    //void setOpenRows(int open_rows) { m_open_rows = open_rows; }
    //void setADCPrecision(int adc_precision) { m_adc_precision = adc_precision; }
    //void setWeightPrecision(int weight_precision) { m_weight_precision = weight_precision; }
    //void setInputPrecision(int input_precision) { m_input_precision = input_precision; }
    //void setVDD(float vdd) { m_vdd = vdd; }
    //void setOutputNoise(float output_noise) { m_output_noise = output_noise; }
    //void setResParasitic(float r_parasitic) { m_r_parasitic = r_parasitic; }
    //void setStressTime(float stress_time) { m_stress_time = stress_time; }
    //void setCalcBER(bool calc_BER) { m_calc_BER = calc_BER; }
    
    void setResDivider(torch::Tensor res_divider) { m_res_divider = res_divider; }
    void setVRef(torch::Tensor vref) { m_vref = vref; }
    void setNumRefs(float num_refs) { m_num_refs = num_refs; }
};
