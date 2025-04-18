#include "gausFilter.h"
#include "canny.h"

#include "string.h"
#include <iostream>
#include <stdlib.h>
#include <array>

using namespace std;
int main() {

    string nameIn = "lena_gray.bmp";
    string pwd = "/nethome/sdatir3/FPGA_Sobel/";
    string pwdIn = pwd + "picIn/";
    string pwdOut = pwd + "picOut/";

    cout << "start processing" << endl;

    if (readImg(pwdIn + nameIn))
    {
        cout << "opened file complete!" << endl;
        // to print a sample 10*10 subimage to see if read correctly
        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < 10; j++)
                cout << img[i][j] << " ";
            cout << endl;
        }
    }
    else
    {
        cout << "opened file failed" << endl;
        return 0;
    }
    if (writeImg(pwdOut + "trial.bmp", img))
    {
        cout << "ouput successful" << endl;
    }

    gausFilter(img);

    gradientForm(img,1);

    nms(magGrad, dirGrad);

    int num_zero=0;
    for (int i = 0; i <H; i++)
    {
        for (int j = 0; j < W; j++){
            if (magGradOut[i][j] == 0) {
                num_zero++;
            }
        }
    }

    histoBuild(magGradOut);
    fill(magGradOut,magGradOut3,1,5);

    thresHolding(magGradOut3,0,30);

    if (writeImg(pwdOut + "output.bmp", magGradOut3))
    {
        cout << "30% p-Tile ouput successful" << endl;
    }

    cout << "end processing" << endl;

	return 0;
}
