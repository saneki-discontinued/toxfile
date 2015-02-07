#ifndef TOXDUMP_H
#define TOXDUMP_H

#include <stdbool.h>
#include "../toxfile_state.h"

#define TOXDUMP_FORMAT_NONE 0
#define TOXDUMP_FORMAT_JSON 1

typedef struct __toxdump_args_t
{
	int format;
	char *profile_name;
	char *toxfile_path;
	bool hex_uppercase;
	bool include_priv_key;
	bool no_newline;
	bool print_help;
} toxdump_args_t;

#define TOXDUMP_INIT_ARGS { TOXDUMP_FORMAT_NONE, NULL, NULL, false, false, false, false }

int main(int argc, char *argv[]);
void parse_args(int argc, char *argv[], toxdump_args_t *args);
int perform(toxdump_args_t *args);
void print_help();

#endif
