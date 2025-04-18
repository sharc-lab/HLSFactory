//
//  main.cpp
//  cannyEdgeDectector
//
//  Created by syd on 2018/10/31.
//  Copyright © 2018年 syd. All rights reserved.
//
#include <iostream>
#include <stdlib.h>
#include "main.h"

using namespace std;
int main(int argc, char* argv[])
{
    char ch;
    //picture address，ready to be changed by Bash Script
    string nameIn = "zebra.bmp";
    string pwd = "/Users/syd/Desktop/c_proj/cannyEdgeDectector/cannyEdgeDectector/";
    string pwdIn = pwd + "picIn/";
    string pwdOut = pwd+ "picOut/";
    cout << "start processing" << endl;
    if (readImg(pwdIn+nameIn))
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
    if (writeImg(pwdOut+"trial.bmp", img))
    {
        cout << "ouput successful" << endl;
    }
    //Function: guas filter
    if (gausFilter(img))
    {
        cout << "gaus filtering successful" << endl;
        
        if (writeImg(pwdOut+"normalized.bmp", img))
        {
            cout << "gaus filtering ouput successful" << endl;
        }
    }
    
    //Function: gradient magnitude and direction computing
    gradientForm(img,1);
    
    
    if (writeImg(pwdOut+"magGrad.bmp", magGrad))
    {
        cout << "ouput Grad mag successful" << endl;
    }
    
    if (writeImg(pwdOut+"magGradX.bmp", magGradX) && writeImg(pwdOut+"magGradY.bmp", magGradY))
    {
        cout << "ouput Grad mag X and Y successful" << endl;
    }
    
    //
    nms(magGrad, dirGrad);//nms
    
    //output to magGradOut
    
    if (writeImg(pwdOut+"magGradAfterNMS.bmp", magGradOut))
    {
        cout << "NMS ouput successful" << endl;
    }
    
    int num_zero=0;
    for (int i = 0; i <H; i++)
    {
        for (int j = 0; j < W; j++)
            if (magGradOut[i][j] == 0) {
                num_zero++;
            }
    }
    
    histoBuild(magGradOut);//build histogram array.
    fill(magGradOut,magGradOut5,1,5);
    fill(magGradOut,magGradOut3,1,5);
    fill(magGradOut,magGradOut1,1,5);
    
    thresHolding(magGradOut5, 0, 50);//threshold 0.5
    
    if (writeImg(pwdOut+"magGradPt0_5.bmp", magGradOut5))
    {
        cout << "50% p-Tile ouput successful" << endl;
    }
    
    thresHolding(magGradOut3, 0, 30);//threshold 0.3
    
    if (writeImg(pwdOut+"magGradPt0_3.bmp", magGradOut3))
    {
        cout << "30% p-Tile ouput successful" << endl;
    }
    
    thresHolding(magGradOut1,0,10);//threshold 0.1
    
    if (writeImg(pwdOut+"magGradPt0_1.bmp", magGradOut1))
    {
        cout << "10% p-Tile ouput successful" << endl;
    }
    cout<<"press any key to end:"<<endl;
    cin >> ch;
}

