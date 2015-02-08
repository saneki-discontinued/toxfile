#ifndef HEX_H
#define HEX_H

#include <stdint.h>
#include <stdlib.h>

// Flags
#define HEX_UPPERCASE (1<<0)

// Determines how from_hex acts when encountering a non-hex
// character. If set, the offending character will be ignored
// and replaced with 0.
#define UNHEX_GENEROUS (1<<0)

// Error code returned when from_hex encounters a non-hex
// character and UNHEX_GENEROUS is not set.
#define UNHEX_NONHEXCHAR (-10)

int to_hex(uint8_t *dest, uint8_t *src, size_t srclen, size_t flags);
int from_hex(uint8_t *dest, uint8_t *src, size_t srclen, size_t flags);

#endif // HEX_H
