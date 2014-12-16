#include <stdbool.h>
#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <tox/tox.h>
#include "toxdump.h"
#include "toxdump_json.h"

toxdump_module g_json_module = TOXDUMP_INIT_MODULE;

int main(int argc, char *argv[])
{
	toxdump_args args = TOXDUMP_INIT_ARGS;
	parse_args(argc, argv, &args);

	init_modules();
	int ret = perform(&args);
	teardown_modules();

	return ret;
}

void parse_args(int argc, char *argv[], toxdump_args *args)
{
	int c, longopt_index = 0;
	struct option longopts[] =
	{
		{ "help", no_argument, 0, 'h' },
		{ 0, 0, 0, 0 }
	};

	while((c = getopt_long(argc, argv, "h?", longopts, &longopt_index)) != -1)
	{
		switch(c)
		{
			case 'h':
			case '?':
				args->print_help = true;
				break;
		}
	}
}

int perform(toxdump_args *args)
{
	if(args->print_help)
	{
		print_help();
		return 0;
	}

	return 0;
}

void teardown_modules()
{
	toxdump_teardown_json_module();
}

void init_modules()
{
	toxdump_create_json_module(&g_json_module);
}

void print_help()
{
	printf("toxdump - dump tox file to some format\n");
	print_help_modules();
	printf("usage:\n");
	printf("  -h, -?, --help    display this usage message\n");
}

void print_help_modules()
{
	printf("modules:");
	if(is_module_loaded(&g_json_module))
	{
		printf(" json (libjansson)");
	}
	printf("\n");
}

bool is_module_loaded(toxdump_module *module)
{
	if(module != NULL && module->func != NULL)
	{
		return true;
	}
	return false;
}
