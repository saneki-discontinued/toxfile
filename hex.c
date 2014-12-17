#include <stdint.h>
#include <stdlib.h>
#include "hex.h"

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

int from_hex(uint8_t *dest, uint8_t *src, size_t srclen, size_t flags)
{
	// Todo
	return 0;
}
