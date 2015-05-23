/*
 * This file is part of toxfile.
 *
 * toxfile is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * toxfile is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with toxfile. If not, see <http://www.gnu.org/licenses/>.
 *
 */

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
	bool include_secret_key;
	bool no_newline;
	bool print_help;
	bool print_version;
} toxdump_args_t;

#define TOXDUMP_INIT_ARGS { TOXDUMP_FORMAT_NONE, NULL, NULL, false, false, false, false, false }

int main(int argc, char *argv[]);
void parse_args(int argc, char *argv[], toxdump_args_t *args);
int perform(toxdump_args_t *args);
void print_help();
void print_version();

#endif
