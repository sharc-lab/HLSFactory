
#include "tb_common.h"

void customInverse(float mat[4][4], float inverseMat[4][4])
{
    int i, j, k;
    float identity[4][4] = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };

    // Forward elimination
    for (i = 0; i < 3 ; i++)
    {
        int pivot = i;
        float pivotsize = mat[i][i];

        if (pivotsize < 0)
            pivotsize = -pivotsize;

        for (j = i + 1; j < 4; j++)
        {
            float tmp = mat[j][i];

            if (tmp < 0)
            {
                tmp = -tmp;
            }

            if (tmp > pivotsize)
            {
                pivot = j;
                pivotsize = tmp;
            }
        }

        if (pivotsize == 0)
        {
            // Cannot invert singular matrix
            customCopy44(identity, inverseMat);
            return;
        }

        if (pivot != i)
        {
            for (j = 0; j < 4; j++)
            {
                float tmp;

                tmp = mat[i][j];
                mat[i][j] = mat[pivot][j];
                mat[pivot][j] = tmp;

                tmp = inverseMat[i][j];
                inverseMat[i][j] = inverseMat[pivot][j];
                inverseMat[pivot][j] = tmp;
            }
        }

        for (j = i + 1; j < 4; j++)
        {
            float f = mat[j][i] / mat[i][i];

            for (k = 0; k < 4; k++)
            {
                mat[j][k] -= f * mat[i][k];
                inverseMat[j][k] -= f * inverseMat[i][k];
            }
        }
    }

    // Backward substitution
    for (i = 3; i >= 0; --i)
    {
        float f;

        if ((f = mat[i][i]) == 0)
        {
            // Cannot invert singular matrix
            customCopy44(identity, inverseMat);
        	return;
        }

        for (j = 0; j < 4; j++)
        {
            mat[i][j] /= f;
            inverseMat[i][j] /= f;
        }

        for (j = 0; j < i; j++)
        {
            f = mat[j][i];

            for (k = 0; k < 4; k++)
            {
                mat[j][k] -= f * mat[i][k];
                inverseMat[j][k] -= f * inverseMat[i][k];
            }
        }
    }
}

void customCopy44(float in[4][4], float out[4][4])
{
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            out[i][j] = in[i][j];
        }
    }
}