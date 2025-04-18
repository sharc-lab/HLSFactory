# **I will open-source my code and host it on my Github. I will email Dr. Hao the link to the repository once set up.**

## Python

It is provided in the form of a jupyter notebook. This code contains a simple PyTorch LSTM model predicting the stock price of IBM. The data used for training is also included.

This portion of the code is primarily based on this excellent Kaggle notebook: https://www.kaggle.com/code/taronzakaryan/predicting-stock-price-using-lstm-model-pytorch/notebook.

## Golden C++

The golden C++ LSTM inference code is written from scratch. A makefile is provided to build the code.

## HLS

A synthesizable HLS LSTM inference with a 30x speedup was implemented. The file ```lstmInference.cpp``` contains the top function, and ```main.cpp``` is the test bench.
