//
// Created by Oscar Gao on 4/22/22.
//

#ifndef LSTM_INFERENCE_TRANSLATION_LSTM_UNIT_H
#define LSTM_INFERENCE_TRANSLATION_LSTM_UNIT_H

#include <vector>

class LSTM_Unit {
public:
    void performInference(double, double, double, double);
    static double sigmoid (double);
    double get_hidden_state_(void) const;
private:
    double hidden_state_{0};
    double cell_state_{0};
};

#endif //LSTM_INFERENCE_TRANSLATION_LSTM_UNIT_H