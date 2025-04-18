
#include "tb_common.h"

void customInverse(fixed_t mat[4][4], fixed_t inverseMat[4][4])
{
    int i, j, k;
    fixed_t identity[4][4] = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };

    // Forward elimination
    for (i = 0; i < 3 ; i++)
    {
        int pivot = i;
        fixed_t pivotsize = mat[i][i];

        if (pivotsize < 0)
            pivotsize = -pivotsize;

        for (j = i + 1; j < 4; j++)
        {
            fixed_t tmp = mat[j][i];

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
                fixed_t tmp;

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
            fixed_t f = mat[j][i] / mat[i][i];

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
        fixed_t f;

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

void customCopy44(fixed_t in[4][4], fixed_t out[4][4])
{
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            out[i][j] = in[i][j];
        }
    }
}

void getPrimitive(
    fixed_t P[MAX_VERT_INDEX][3],
    uint32_t trisIndex[NUM_TRIS * 3],
    fixed_t v0Arr[3], fixed_t v1Arr[3], fixed_t v2Arr[3],
    uint32_t index)
{
    uint32_t j = index*3;
    for (int i = 0; i < 3; ++i)
    {
        v0Arr[i] = P[trisIndex[j]][i];
        v1Arr[i] = P[trisIndex[j + 1]][i];
        v2Arr[i] = P[trisIndex[j + 2]][i];
    }
}