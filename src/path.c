#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include <libsy.h>
#include "path.h"

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
	pjoin(temp, homepath, TOXFILE_NIX_HPATH_VENOM, sizeof(temp));
	if(stat(temp, &st) == 0 && S_ISREG(st.st_mode))
	{
		strncpy(dest, temp, destlen);
		return;
	}

	memset(temp, 0, sizeof(temp));

	// Check default uTox path for current user
	pjoin(temp, homepath, TOXFILE_NIX_HPATH_UTOX, sizeof(temp));
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

	pjoin(dirpath, homepath, TOXFILE_NIX_HPATH_DIR, sizeof(dirpath));
	pjoin(fullpath, dirpath, userfile, sizeof(fullpath));

	if(stat(fullpath, &st) == 0 && S_ISREG(st.st_mode))
	{
		strncpy(dest, fullpath, destlen);
		return true;
	}

	return false;
}
