#include <cstdio>
#include <cstring>
#include <vector>
#include "sgd.h"

int main() {
  // Large training buffers must not live on the stack (csim SIGSEGV).
  std::vector<VectorDataType> data(NUM_FEATURES * NUM_TRAINING / D_VECTOR_SIZE);
  std::vector<VectorLabelType> label(NUM_TRAINING / L_VECTOR_SIZE);
  std::vector<VectorFeatureType> theta(NUM_FEATURES / F_VECTOR_SIZE);

  std::memset(data.data(), 0, data.size() * sizeof(VectorDataType));
  std::memset(label.data(), 0, label.size() * sizeof(VectorLabelType));
  std::memset(theta.data(), 0, theta.size() * sizeof(VectorFeatureType));

  for (int i = 0; i < (int)data.size(); ++i)
    data[i] = VectorDataType(i * 3 + 1);
  for (int i = 0; i < (int)label.size(); ++i)
    label[i] = VectorLabelType((i & 1) ? 1 : 0);

  for (int epoch = 0; epoch < NUM_EPOCHS; ++epoch)
    SgdLR(data.data(), label.data(), theta.data(), (epoch == 0),
          (epoch == NUM_EPOCHS - 1));

  std::printf("SgdLR csim OK\n");
  return 0;
}
