#ifndef CRC32_H
#define CRC32_H

#include <stdint.h>

#define CRC32_MAX_LEN 1024

typedef uint32_t crc_t;

crc_t crc32buf(const uint8_t buf[CRC32_MAX_LEN], unsigned int len);

#endif
