#include "convolution.h"

//Golden reference to compare outputs
void convolution_golden_reference(int ip_data[IF_LENGTH][IF_WIDTH], int Wt_data[WT_LENGTH][WT_WIDTH], int op_data[OP_LENGTH][OP_WIDTH])
{
    for(int i = 0; i < OP_LENGTH; i++)             // Output Height  
    {      
        for(int j = 0; j < OP_WIDTH; j++)           // Output Width
        {            
            for(int kh = 0; kh < WT_LENGTH; kh++)   // Kernel Height
            {      
                for(int kw = 0; kw < WT_WIDTH; kw++) // Kernel Width   
                { 
                    if (((i*STRIDE+kh-PADDING) >= 0) && ((i*STRIDE+kh-PADDING) < IF_LENGTH) && ((j*STRIDE+kw-PADDING) >= 0) && ((j*STRIDE+kw-PADDING) < IF_WIDTH)){                    
                        if(kh == 0 && kw == 0)  // Initialize output feature
                            op_data[i][j]  = ip_data[i*STRIDE+kh-PADDING][j*STRIDE+kw-PADDING] * Wt_data[kh][kw];
                        else                              // Multiple and Accumulate (MAC)
                            op_data[i][j] += ip_data[i*STRIDE+kh-PADDING][j*STRIDE+kw-PADDING] * Wt_data[kh][kw];
                    }
                    else{
                        if( kh == 0 && kw == 0)  // Initialize output feature
                            op_data[i][j]  = 0;
                    }
                }
            }
        }
    }
}

int main()
{   
    int ip_data[IF_LENGTH][IF_WIDTH];
    int Wt_data[WT_LENGTH][WT_WIDTH];
    int op_data[OP_LENGTH][OP_WIDTH];
    int golden_op_data[OP_LENGTH][OP_WIDTH];

    int pe_ip_data[BUFFER_SIZE-1+WT_LENGTH][IF_WIDTH];
    int pe_op_data[BUFFER_SIZE][OP_WIDTH];

    int flag;

    for (int i = 0; i < IF_LENGTH; i++)
    {       
        for (int j = 0; j < IF_WIDTH; j++)
        {             
            ip_data[i][j] = rand() % 50;;
        }
    }

    for (int i = 0; i < WT_LENGTH; i++)
    {       
        for (int j = 0; j < WT_WIDTH; j++)
        {               
            Wt_data[i][j] = rand() % 50;
        }
    }

    //FOLDING logic: mapping and then running multiple convolution primitives from different logical PEs on the same physical PE
    if(OP_LENGTH > BUFFER_SIZE)
    {
        for(int k = 0; k < OP_LENGTH; k = k+BUFFER_SIZE)
        {
            for (int i = 0; i < (BUFFER_SIZE-1+WT_LENGTH); i++)       
            {
                for (int j = 0; j < IF_WIDTH; j++)
                {
                    //create new input feature map array to functional fit into a 3x3 PE array: subset of the bigger input feature map array
                    if((i+k) < IF_LENGTH)
                        pe_ip_data[i][j] = ip_data[i+k][j];
                    else
                        pe_ip_data[i][j] = 0;
                }
            }
                // Pass different set of input feature maps to 3x3 PE array multiple times to implement folding
                convolution(pe_ip_data, Wt_data, pe_op_data);
                reset();

            for (int i = 0; i < BUFFER_SIZE; i++)       
                for (int j = 0; j < OP_WIDTH; j++)
                {
                    if((i+k) < OP_LENGTH)
                        op_data[i+k][j] = pe_op_data[i][j];
                }
        }
    }
    else
        convolution(ip_data, Wt_data, op_data);

    convolution_golden_reference(ip_data, Wt_data, golden_op_data);
    flag = 0;

    cout << "ROW STATIONARY CONVOLUTION OUTPUT: " << "\n";
    for (int i = 0; i < OP_LENGTH; i++)
    {
        for (int j = 0; j < OP_WIDTH; j++)
        {
            if(op_data[i][j] != golden_op_data[i][j])
                flag = 1;
            cout << op_data[i][j] << " ";
        }
        cout << "\n";
    }

    if(flag == 0)
    {
        cout << "-----------------------------------\n";
        cout << "|         TEST PASSED!            |\n";
        cout << "-----------------------------------\n";
    }       
    else
    {
        cout << "-----------------------------------\n";
        cout << "|         TEST FAILED!            |\n";
        cout << "-----------------------------------\n";
    }
}
