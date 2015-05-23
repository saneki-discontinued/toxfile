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

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "jansson_exports.h"

void toxdump_close_jansson_exports(jansson_exports_t *exports)
{
	if(exports != NULL && exports->handle != NULL)
	{
		dlclose(exports->handle);
		exports->handle = NULL;
	}
}

bool toxdump_open_jansson_exports(jansson_exports_t *exports)
{
	char *dlsym_error;

	if(exports == NULL)
	{
		fprintf(stderr, "no exports to set\n");
		return false;
	}

	void *lib = dlopen("libjansson.so.4", RTLD_LAZY);

	if(lib == NULL)
	{
		fprintf(stderr, "dlopen error: %s\n", dlerror());
		return false;
	}

	dlerror();

	exports->handle = lib;

#define JANSSON_EXPORT(SYMBOL) {\
	exports->SYMBOL = dlsym(lib, #SYMBOL);\
	dlsym_error = dlerror();\
	if(dlsym_error != NULL) {\
		fprintf(stderr, "dlsym error (%s): %s\n", #SYMBOL, dlsym_error);\
		dlclose(lib);\
		return false;\
	}\
}

	JANSSON_EXPORT(json_object);
	JANSSON_EXPORT(json_stringn);
	JANSSON_EXPORT(json_integer);
	JANSSON_EXPORT(json_dumpf);
	JANSSON_EXPORT(json_object_set_new);

#undef JANSSON_EXPORT

	return true;
}
