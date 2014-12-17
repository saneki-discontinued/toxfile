#ifndef TOXDUMP_H
#define TOXDUMP_H

#include <stdbool.h>
#include "../toxfile_state.h"

#define TOXDUMP_FORMAT_NONE 0
#define TOXDUMP_FORMAT_JSON 1

typedef struct __toxdump_args
{
	int format;
	char *toxfile_path;
	bool print_help;
} toxdump_args;

#define TOXDUMP_INIT_ARGS { TOXDUMP_FORMAT_NONE, false, NULL }

int main(int argc, char *argv[]);
void parse_args(int argc, char *argv[], toxdump_args *args);
int perform(toxdump_args *args);
void print_help();

#endif
