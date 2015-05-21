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

#ifndef PATH_H
#define PATH_H

#include <stddef.h>

// Default paths to save file, relative to home (on *nix systems)
#define TOXFILE_NIX_HPATH_DIR ".config/tox" // Default tox directory path
#define TOXFILE_NIX_HPATH_VENOM ".config/tox/data"
#define TOXFILE_NIX_HPATH_UTOX ".config/tox/tox_save"

void toxfile_try_find_save_path(char *dest, size_t destlen);
bool toxfile_try_get_profile_path(char *dest, size_t destlen, char *username);

#endif // PATH_H
