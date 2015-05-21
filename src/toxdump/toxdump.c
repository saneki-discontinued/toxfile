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
#include <string.h>
#include <getopt.h>
#include <linux/limits.h>
#include <unistd.h>
#include <tox/tox.h>
#include <libsy.h>
#include "toxdump.h"
#include "toxdump_json.h"
#include "../toxfile_state.h"
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
		{ "include-private-key", no_argument, 0, 'x' },
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
				args->include_priv_key = true;
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
		// Make this better
		Tox *tox = tox_new(NULL);
		if(tox == NULL) return -1;
		FILE *infile = fopen(path, "rb");
		if(infile == NULL) return -1;

		int64_t savesize = fsize(infile);
		uint8_t savedata[savesize];
		fread(savedata, savesize, 1, infile);
		fclose(infile);

		int loadret = tox_load(tox, savedata, savesize);
		if(loadret != 0) return -1;

		FILE *outfile = stdout;

		int state_flags = (args->include_priv_key ? TOXFILE_LOAD_PRIVKEY : 0);
		toxfile_state_t state;
		toxfile_load_state(tox, &state, state_flags);

		if(args->format == TOXDUMP_FORMAT_NONE
		|| args->format == TOXDUMP_FORMAT_JSON)
		{
			toxdump_perform_json(&state, outfile, args);
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
	printf("  -x, --include-private-key    include private key in the output\n");
	printf("  -X, --hex-uppercase          dump hex strings in uppercase\n");
	printf("  -h, -?, --help               display this usage message\n");
	printf("  -v, --version                print toxdump version\n");
}

void print_version()
{
	printf("toxdump v%s\n", TOXFILE_PROJ_VERSION);
}
