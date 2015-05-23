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

#ifndef TOXDUMP_JANSSON_EXPORTS_H
#define TOXDUMP_JANSSON_EXPORTS_H

#include <stdbool.h>
#include <jansson.h>

typedef struct jansson_exports_t
{
	void *handle;
	json_t *(*json_object)();
	json_t *(*json_stringn)(const char *, size_t);
	json_t *(*json_integer)(json_int_t);
	int (*json_dumpf)(const json_t *, FILE *, size_t);
	int (*json_object_set_new)(json_t *, const char *, json_t *);
} jansson_exports_t;

void toxdump_close_jansson_exports(jansson_exports_t *exports);
bool toxdump_open_jansson_exports(jansson_exports_t *exports);

#endif // TOXDUMP_JANSSON_EXPORTS_H
