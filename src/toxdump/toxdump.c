#include <stdbool.h>
#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <tox/tox.h>
#include "toxdump.h"
#include "toxdump_json.h"
#include "../toxfile_state.h"
#include "../file.h"

int main(int argc, char *argv[])
{
	toxdump_args args = TOXDUMP_INIT_ARGS;
	parse_args(argc, argv, &args);
	int ret = perform(&args);
	return ret;
}

void parse_args(int argc, char *argv[], toxdump_args *args)
{
	int c, index, longopt_index = 0;
	struct option longopts[] =
	{
		{ "help", no_argument, 0, 'h' },
		{ "json", no_argument, 0, 'j' },
		{ 0, 0, 0, 0 }
	};

	while((c = getopt_long(argc, argv, "hj?", longopts, &longopt_index)) != -1)
	{
		switch(c)
		{
			case 'j':
				args->format = TOXDUMP_FORMAT_JSON;
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

int perform(toxdump_args *args)
{
	if(args->print_help)
	{
		print_help();
		return 0;
	}

	if(args->toxfile_path != NULL)
	{
		// Make this better
		Tox *tox = tox_new(NULL);
		if(tox == NULL) return -1;
		FILE *infile = fopen(args->toxfile_path, "rb");
		if(infile == NULL) return -1;

		int64_t savesize = fsize(infile);
		uint8_t savedata[savesize];
		fread(savedata, savesize, 1, infile);
		fclose(infile);

		int loadret = tox_load(tox, savedata, savesize);
		if(loadret != 0) return -1;

		FILE *outfile = stdout;

		toxfile_state_t state;
		toxfile_load_state(tox, &state, 0);

		if(args->format == TOXDUMP_FORMAT_JSON)
		{
			toxdump_perform_json(&state, outfile);
		}
	}

	return 0;
}

void print_help()
{
	printf("toxdump - dump tox file to some format\n");
	printf("usage: toxdump [-hj?] [toxfile]\n");
	printf("  -j, --json        dump to JSON\n");
	printf("  -h, -?, --help    display this usage message\n");
}
