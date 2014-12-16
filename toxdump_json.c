#include <dlfcn.h>
#include <jansson.h>
#include <tox/tox.h>
#include "toxdump.h"
#include "toxdump_json.h"

void *jansson_handle = NULL;

void toxdump_create_json_module(toxdump_module *module)
{
	jansson_handle = dlopen("libjansson.so", RTLD_LAZY);
	if(!jansson_handle)
	{
		return;
	}

	// Todo: Set global variable function pointers

	module->func = toxdump_perform_json;
}

int toxdump_perform_json(Tox *tox, FILE *file)
{
	return 0;
}

void toxdump_teardown_json_module()
{
	if(jansson_handle)
	{
		dlclose(jansson_handle);
	}
}
