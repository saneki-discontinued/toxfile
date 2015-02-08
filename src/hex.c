#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "hex.h"

// 7 bit ascii hex character -> 4 bit value
#define HVAL(c) ((c) >= '0' && (c) <= '9' ? ((c) - '0') : \
		((c) >= 'A' && (c) <= 'F' ? ((c) - ('A' - 10)) : \
		((c) >= 'a' && (c) <= 'f' ? ((c) - ('a' - 10)) : \
		0 )))

// Check if a char is hex
#define ISHEX(c) (((c) >= '0' && (c) <= '9') || ((c) >= 'A' && (c) <= 'F') || ((c) >= 'a' && (c) <= 'f'))

/**
 * Convert some buffer into a hex string. Destination buffer should be
 * at least twice as large as srclen.
 * @param dest Destination buffer
 * @param src Source buffer
 * @param srclen Source buffer size
 * @param flags Option flags
 */
int to_hex(uint8_t *dest, uint8_t *src, size_t srclen, size_t flags)
{
	size_t i, j;
	uint8_t b, c[2];

	if(dest == NULL || src == NULL)
	{
		return -1;
	}

	for(i = 0; i < srclen; i++)
	{
		b = src[i];

		c[0] = b / 0x10;
		c[1] = b % 0x10;

		for(j = 0; j < 2; j++)
		{
			if(c[j] < 0xA)
			{
				c[j] += '0';
			}
			else
			{
				if((flags & HEX_UPPERCASE) == HEX_UPPERCASE)
				{
					c[j] = (c[j] - 0xA) + 'A';
				}
				else
				{
					c[j] = (c[j] - 0xA) + 'a';
				}
			}
		}

		dest[(i*2)] = c[0];
		dest[(i*2)+1] = c[1];
	}

	return 0;
}

/**
 * Convert some hex string into a buffer. Destination buffer should be
 * at least half as large as srclen.
 * @param dest Destination buffer
 * @param src Source hex buffer
 * @param srclen Size of source buffer, if 0 will be assumed null-terminating
 * @param flags Option flags
 * @return -1 on error
 */
int from_hex(uint8_t *dest, uint8_t *src, size_t srclen, size_t flags)
{
	int i = 0;

	if(dest == NULL || src == NULL) return -1;

	if(srclen == 0) srclen = strlen(src);

	// Assume an omitted leading zero if odd length
	if(srclen % 2 != 0)
	{
		dest[0] = HVAL(src[0]);
		srclen--;
		src++;
		dest++;
	}

	for(i = 0; i < (srclen / 2); i++)
	{
		char c1 = src[i*2], c2 = src[(i*2)+1];

		// If non-hex char and not generous, error
		if((!ISHEX(c1) || !ISHEX(c2))
		&& (flags & UNHEX_GENEROUS) == 0)
		{
			return UNHEX_NONHEXCHAR;
		}

		dest[i] = (HVAL(c1) << 4) | (HVAL(c2));
	}

	return 0;
}
