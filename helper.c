#include <stdio.h>
#include <stdint.h>

int64_t fsize(FILE *file)
{
	if(file == NULL)
		return -1;

	int64_t cur = ftell(file);
	fseek(file, 0, SEEK_END);
	int64_t size = ftell(file);
	fseek(file, cur, SEEK_SET);

	return size;
}
