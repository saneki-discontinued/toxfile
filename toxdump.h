#ifndef TOXDUMP_H
#define TOXDUMP_H

#include <stdbool.h>

typedef int (*toxdump_dump_function)(Tox *tox, FILE *file);

typedef struct __toxdump_module
{
	toxdump_dump_function func;
} toxdump_module;

#define TOXDUMP_INIT_MODULE { NULL }

typedef struct __toxdump_args
{
	bool print_help;
} toxdump_args;

#define TOXDUMP_INIT_ARGS { false }

int main(int argc, char *argv[]);
void init_modules();
bool is_module_loaded(toxdump_module *module);
void parse_args(int argc, char *argv[], toxdump_args *args);
int perform(toxdump_args *args);
void print_help();
void print_help_modules();
void teardown_modules();

#endif
