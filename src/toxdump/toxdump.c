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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <linux/limits.h>
#include <unistd.h>
#include <tox/tox.h>
#include <libsy.h>
#include "toxdump.h"
#include "toxdump_json.h"
#include "../toxfile_state.h"
#include "../toxfile_util.h"
#include "../path.h"
#include "../version.h"

int main(int argc, char *argv[])
{
	toxdump_args_t args = TOXDUMP_INIT_ARGS;
	parse_args(argc, argv, &args);
	int ret = perform(&args);
	return ret;
}

void parse_args(int argc, char *argv[], toxdump_args_t *args)
{
	int c, index, longopt_index = 0;
	extern char *optarg;
	struct option longopts[] =
	{
		{ "help",                no_argument, 0, 'h' },
		{ "hex-uppercase",       no_argument, 0, 'X' },
		{ "json",                no_argument, 0, 'j' },
		{ "include-secret-key",  no_argument, 0, 'x' },
		{ "no-newline",          no_argument, 0, 'n' },
		{ "profile",             required_argument, 0, 'p' },
		{ "version",             no_argument, 0, 'v' },
		{ 0, 0, 0, 0 }
	};

	while((c = getopt_long(argc, argv, "hjnp:vxX?", longopts, &longopt_index)) != -1)
	{
		switch(c)
		{
			case 'j':
				args->format = TOXDUMP_FORMAT_JSON;
				break;

			case 'n':
				args->no_newline = true;
				break;

			case 'p':
				args->profile_name = optarg;
				break;

			case 'v':
				args->print_version = true;
				break;

			case 'x':
				args->include_secret_key = true;
				break;

			case 'X':
				args->hex_uppercase = true;
				break;

			case 'h':
			case '?':
				args->print_help = true;
				break;
		}
	}

	for(index = optind; index < argc; index++)
	{
		// Only care about first, which would be the toxfile
		args->toxfile_path = argv[index];
		break;
	}
}

int perform(toxdump_args_t *args)
{
	if(args->print_help)
	{
		print_help();
		return 0;
	}

	if(args->print_version)
	{
		print_version();
		return 0;
	}

	char path[PATH_MAX];
	memset(path, 0, sizeof(path));
	if(args->toxfile_path != NULL)
	{
		strncpy(path, args->toxfile_path, sizeof(path));
	}
	else if(args->profile_name != NULL)
	{
		bool success = toxfile_try_get_profile_path(path, sizeof(path), args->profile_name);
		if(!success)
		{
			fprintf(stderr, "Couldn't find tox profile file for %s\n", args->profile_name);
		}
	}
	else
	{
		toxfile_try_find_save_path(path, sizeof(path));
	}

	if(strlen(path) > 0)
	{
		TOXFILE_ERR_OPEN error;
		Tox *tox = toxfile_open(path, &error);
		if(error != TOXFILE_ERR_OPEN_OK && error != TOXFILE_ERR_OPEN_OK_ENCRYPTED)
		{
			fprintf(stderr, "toxfile_open error: %i\n", error);
			exit(EXIT_FAILURE);
		}

		int state_flags = (args->include_secret_key ? TOXFILE_LOAD_SECRET_KEY : 0);
		toxfile_state_t state;
		toxfile_load_state(tox, &state, state_flags);

		if(args->format == TOXDUMP_FORMAT_NONE
		|| args->format == TOXDUMP_FORMAT_JSON)
		{
			if(toxdump_json_dlopen())
			{
				toxdump_json_perform(&state, stdout, args);
				toxdump_json_dlclose();
			}
		}

		if(!args->no_newline)
		{
			printf("\n");
		}
	}
	else if(args->toxfile_path == NULL && args->profile_name == NULL)
	{
		fprintf(stderr, "Was not able to find a tox file at the default paths\n");
	}

	return 0;
}

void print_help()
{
	printf("toxdump - dump tox file to some format\n");
	printf("usage: toxdump [options] <file>\n");
	printf("  -j, --json                   dump to JSON (default)\n");
	printf("  -n, --no-newline             don't print a newline\n");
	printf("  -p, --profile                profile to try and find tox file with\n");
	printf("  -x, --include-secret-key     include secret key in the output\n");
	printf("  -X, --hex-uppercase          dump hex strings in uppercase\n");
	printf("  -h, -?, --help               display this usage message\n");
	printf("  -v, --version                print toxdump version\n");
}

void print_version()
{
	printf("toxdump v%s\n", TOXFILE_PROJ_VERSION);
}
