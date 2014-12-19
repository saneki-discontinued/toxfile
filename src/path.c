#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include "path.h"

void joinpath(char *dest, const char *part1, const char *part2, uint32_t maxlen)
{
	if(dest == NULL)
		return;

	if(maxlen == 0)
		maxlen = PATH_MAX;

	if(part1 != NULL && part2 == NULL)
	{
		strncpy(dest, part1, maxlen - 1);
		return;
	}
	else if (part1 == NULL && part2 != NULL)
	{
		strncpy(dest, part2, maxlen - 1);
		return;
	}

	//if((strlen(part1) + strlen(part2) + 1) > maxlen)
	//{
	//	return;
	//}

	// Issue: if part2 is really long, could buffer overflow

	int p1len = strlen(part1);

	char sep = '/'; // Assume *nix for now

	strcpy(dest, part1);

	if(part1[p1len-1] == sep && part2[0] == sep)
	{
		strcpy(dest+p1len, part2+1);
	}
	else if(part1[p1len-1] != sep && part2[0] != sep)
	{
		strcpy(dest+p1len, "/");
		strcpy(dest+p1len+1, part2);
	}
	else
	{
		strcpy(dest+p1len, part2);
	}
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
