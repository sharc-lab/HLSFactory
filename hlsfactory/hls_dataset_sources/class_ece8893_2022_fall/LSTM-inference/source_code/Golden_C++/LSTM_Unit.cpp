//
// Created by Oscar Gao on 4/22/22.
//

#include "LSTM_Unit.h"
#include <cmath>

/**
 * Perform forward inference
 *
 * Weights are in the order of i, f, c, o
 *
 * @param i
 * @param f
 * @param c
 * @param o
 *
 * @return NONE; Modify cell_state_ and hidden_state_
 */
void LSTM_Unit::performInference(double i, double f, double c, double o)
{
    double f_r = sigmoid(f);
    double i_r = sigmoid(i);
    double o_r = sigmoid(o);
    this->cell_state_ = f_r * this->cell_state_ + tanh(c) * i_r;
    this->hidden_state_ = o_r * tanh(this->cell_state_);
}

/**
 * Sigmoid in-house
 *
 * Weights are in the order of i, f, c, o
 *
 * @param x the input
 * @return result of the sigmoid function
 */
double LSTM_Unit::sigmoid(double x)
{
    return (1/(1+exp(-x)));
}

/**
 * Getter for hidden_state_
 *
 * @return hidden_state_ private variable
 */
double LSTM_Unit::get_hidden_state_() const
{
    return this->hidden_state_;
}
