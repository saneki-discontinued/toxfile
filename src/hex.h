#ifndef HEX_H
#define HEX_H

#include <stdint.h>
#include <stdlib.h>

// Flags
#define HEX_UPPERCASE (1<<0)

int to_hex(uint8_t *dest, uint8_t *src, size_t srclen, size_t flags);
int from_hex(uint8_t *dest, uint8_t *src, size_t srclen, size_t flags);

#endif // HEX_H
