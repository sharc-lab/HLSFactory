#include <cstdio>
#include "digitrec.h"
#include "training_data.h"
#include "testing_data.h"

int main() {
  WholeDigitType training_in0[NUM_TRAINING / 2];
  WholeDigitType training_in1[NUM_TRAINING / 2];
  WholeDigitType test_in[NUM_TEST];
  LabelType result[NUM_TEST];

  for (int i = 0; i < NUM_TRAINING / 2; ++i) {
    training_in0[i].range(63, 0) = training_data[i * DIGIT_WIDTH + 0];
    training_in0[i].range(127, 64) = training_data[i * DIGIT_WIDTH + 1];
    training_in0[i].range(191, 128) = training_data[i * DIGIT_WIDTH + 2];
    training_in0[i].range(255, 192) = training_data[i * DIGIT_WIDTH + 3];
  }
  for (int i = 0; i < NUM_TRAINING / 2; ++i) {
    const int base = (NUM_TRAINING / 2 + i) * DIGIT_WIDTH;
    training_in1[i].range(63, 0) = training_data[base + 0];
    training_in1[i].range(127, 64) = training_data[base + 1];
    training_in1[i].range(191, 128) = training_data[base + 2];
    training_in1[i].range(255, 192) = training_data[base + 3];
  }
  for (int i = 0; i < NUM_TEST; ++i) {
    test_in[i].range(63, 0) = testing_data[i * DIGIT_WIDTH + 0];
    test_in[i].range(127, 64) = testing_data[i * DIGIT_WIDTH + 1];
    test_in[i].range(191, 128) = testing_data[i * DIGIT_WIDTH + 2];
    test_in[i].range(255, 192) = testing_data[i * DIGIT_WIDTH + 3];
  }

  DigitRec(training_in0, test_in, result, 0);
  DigitRec(training_in1, test_in, result, 1);
  std::printf("DigitRec csim OK (result0=%u)\n", (unsigned)result[0]);
  return 0;
}
