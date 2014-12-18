#ifndef PATH_H
#define PATH_H

#include <stdint.h>

// Default paths to save file, relative to home (on *nix systems)
#define TOXFILE_NIX_HPATH_VENOM ".config/tox/data"
#define TOXFILE_NIX_HPATH_UTOX ".config/tox/tox_save"

void joinpath(char *dest, const char *part1, const char *part2, uint32_t maxlen);
void toxfile_try_find_save_path(char *dest, size_t destlen);

#endif // PATH_H
