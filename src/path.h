#ifndef PATH_H
#define PATH_H

#include <stdbool.h>
#include <stdint.h>

// Default paths to save file, relative to home (on *nix systems)
#define TOXFILE_NIX_HPATH_DIR ".config/tox" // Default tox directory path
#define TOXFILE_NIX_HPATH_VENOM ".config/tox/data"
#define TOXFILE_NIX_HPATH_UTOX ".config/tox/tox_save"

void joinpath(char *dest, const char *part1, const char *part2, uint32_t maxlen);
void toxfile_try_find_save_path(char *dest, size_t destlen);
bool toxfile_try_get_profile_path(char *dest, size_t destlen, char *username);

#endif // PATH_H
