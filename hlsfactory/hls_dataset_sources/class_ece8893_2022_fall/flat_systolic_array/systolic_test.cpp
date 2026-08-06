#include <iostream>
#include <cstdlib>
#include <iostream>
#include "systolic.h"

using namespace std;

int main()
{
    // data_t **const query_matrix = new data_t*[COLUMN];
    // data_t **const key_matrix = new data_t*[COLUMN];
    // data_t **const value_matrix = new data_t*[COLUMN];
    // data_t **const output = new data_t*[COLUMN];
    data_t query_matrix[ROW][COLUMN];
    data_t key_matrix[ROW][COLUMN];
    data_t value_matrix[ROW][COLUMN];
    data_t bias_matrix[ROW][COLUMN];
    data_t output[ROW][COLUMN];
    for (int i = 0; i < ROW; ++i)
    {
        for (int j = 0; j < COLUMN; ++j)
        {
        	query_matrix[i][j] = 1;
            key_matrix[i][j] = 2;
            value_matrix[i][j] = 4;
            bias_matrix[i][j] = 3;
            output[i][j] = 0;
        }
    }
    systolic_array(query_matrix, key_matrix, value_matrix, bias_matrix, output);
     for (int i = 0; i < ROW; ++i)
     {
         for (int j = 0; j < COLUMN; ++j)
         {
             if (output[i][j] != (2*1*COLUMN+3)*4*ROW)
             {
                 std::cout <<"Wrong number " << output[i][j] << " at [" << i << ", " << j << "]" << std::endl;
             }
         }
     }
    return 0;
}
