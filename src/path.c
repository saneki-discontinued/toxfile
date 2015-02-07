#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include "path.h"

/**
 * Path join.
 * @param dest Pointer to destination buffer
 * @param part1 Pointer to first part of path, null-terminated
 * @param part2 Pointer to second part of path, null-terminated
 * @param maxlen Size of dest buffer. If 0, will assume a size of PATH_MAX.
 * @return 0 if good, or -1 if error
 */
int joinpath(char *dest, const char *part1, const char *part2, uint32_t maxlen)
{
	if(dest == NULL) return -1;
	if(part1 == NULL && part2 == NULL) return -1;

	if(maxlen == 0) maxlen = PATH_MAX;

	// If only one part is NULL, copy over the other
	if(part1 == NULL)
	{
		strncpy(dest, part2, maxlen);
		return 0;
	}
	else if(part2 == NULL)
	{
		strncpy(dest, part1, maxlen);
		return 0;
	}

	memset(dest, 0, maxlen);

	if(part1 == NULL) part1 = "";
	if(part2 == NULL) part2 = "";

	char sep = '/';

	int len1 = strlen(part1), len2 = strlen(part2);	
	bool sep1 = (len1 > 0 && part1[len1 - 1] == sep),
	     sep2 = (len2 > 0 && part2[0] == sep);

	if(sep1) len1--;
	if(sep2)
	{
		part2++;
		len2--;
	}

	//int full_len = (len1 + len2 + 2);
	//if(full_len > maxlen) full_len = maxlen;

	snprintf(dest, maxlen, "%s%c%s", part1, sep, part2);

	return 0;
}

void toxfile_try_find_save_path(char *dest, size_t destlen)
{
	if(dest == NULL || destlen == 0)
		return;

#if defined(_WIN32) || defined(_WIN64)
	return; // Do nothing for now
#else // Assume *nix

	struct stat st;
	char temp[destlen];
	char *homepath = getenv("HOME");

	// Check default Venom path for current user
	joinpath(temp, homepath, TOXFILE_NIX_HPATH_VENOM, sizeof(temp));
	if(stat(temp, &st) == 0 && S_ISREG(st.st_mode))
	{
		strncpy(dest, temp, destlen);
		return;
	}

	memset(temp, 0, sizeof(temp));

	// Check default uTox path for current user
	joinpath(temp, homepath, TOXFILE_NIX_HPATH_UTOX, sizeof(temp));
	if(stat(temp, &st) == 0 && S_ISREG(st.st_mode))
	{
		strncpy(dest, temp, destlen);
		return;
	}

#endif
}

bool toxfile_try_get_profile_path(char *dest, size_t destlen, char *username)
{
	struct stat st;
	char dirpath[destlen], fullpath[destlen], userfile[destlen];
	char *homepath = getenv("HOME");

	strncpy(userfile, username, sizeof(userfile));
	strncat(userfile, ".tox", sizeof(userfile));

	joinpath(dirpath, homepath, TOXFILE_NIX_HPATH_DIR, sizeof(dirpath));
	joinpath(fullpath, dirpath, userfile, sizeof(fullpath));

	if(stat(fullpath, &st) == 0 && S_ISREG(st.st_mode))
	{
		strncpy(dest, fullpath, destlen);
		return true;
	}

	return false;
}
