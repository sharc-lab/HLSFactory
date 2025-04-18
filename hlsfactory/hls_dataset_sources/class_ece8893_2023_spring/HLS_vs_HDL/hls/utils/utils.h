#ifndef UTILS_H
#define UTILS_H

#include "string.h"
#include <iostream>
#include <fstream>
#include <cstddef>

#define H 512
#define W 512

extern int num_Chn;
extern int isRead;
extern int img[H][W];
extern int out[H][W];

bool readImg(std::string);
bool writeImg(std::string, int(&array)[H][W]);
bool iniArray(size_t*);

#endif
