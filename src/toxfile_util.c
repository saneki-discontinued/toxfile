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

#define SET_ERROR_PARAM(param, x) {if(param != NULL) {*param = x;}}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <libsy.h>

#include "io.h"
#include "toxfile_util.h"

Tox * toxfile_open(const char *savepath, TOXFILE_ERR_OPEN *error)
{
	toxfile_open_ex(savepath, error, NULL, NULL);
}

/**
 * Try to open a toxfile. If encrypted, will prompt for password and try
 * to use that.
 */
Tox * toxfile_open_ex(const char *savepath, TOXFILE_ERR_OPEN *r_error, TOX_ERR_NEW *r_new_error, TOX_ERR_DECRYPTION *r_decrypt_error)
{
	SET_ERROR_PARAM(r_error, TOXFILE_ERR_OPEN_OK);
	SET_ERROR_PARAM(r_new_error, TOX_ERR_NEW_OK);

#ifndef TOXFILE_NO_ENC
	SET_ERROR_PARAM(r_decrypt_error, TOX_ERR_DECRYPTION_OK);
#endif

	if(savepath == NULL)
	{
		SET_ERROR_PARAM(r_error, TOXFILE_ERR_OPEN_NULL);
		return NULL;
	}

	//char savepathbuf[PATH_MAX];

	// If no path passed in args, try to find path
	// Todo: Don't do any default path stuff (in this function)
	//if(savepath == NULL)
	//{
	//	toxfile_try_find_save_path(savepathbuf, sizeof(savepathbuf));
	//
	//	// If still empty string (all 0x00s), couldn't find default file
	//	if(strlen(savepathbuf) == 0)
	//	{
	//		fprintf(stderr, "could not find file at default save path\n");
	//		SET_ERROR_PARAM(r_error, TOXFILE_ERR_OPEN_NO_DEFAULT);
	//		return NULL;
	//	}
	//
	//	savepath = savepathbuf;
	//}
	//else
	//{
	// Make sure path passed via args is at least a regular file
	struct stat st;
	if(stat(savepath, &st) != 0)
	{
		fprintf(stderr, "specified toxfile does not exist: %s\n", savepath);
		SET_ERROR_PARAM(r_error, TOXFILE_ERR_OPEN_FILE_NOT_FOUND);
		return NULL;
	}

	if(!S_ISREG(st.st_mode))
	{
		fprintf(stderr, "specified toxfile is not regular file: %s\n", savepath);
		SET_ERROR_PARAM(r_error, TOXFILE_ERR_OPEN_FILE_NOT_REGULAR);
		return NULL;
	}
	//}

	// Store opened path
	//args->opened_path = savepath;

	// Try opening at savepath
	FILE *savefile = fopen(savepath, "rb");

	if(savefile == NULL)
	{
		fprintf(stderr, "error opening tox savefile: %s\n", savepath);
		SET_ERROR_PARAM(r_error, TOXFILE_ERR_OPEN_FILE);
		return NULL;
	}

	// Get the file size and make sure no error occurred
	int64_t _savesize = fsize(savefile);
	if(_savesize < 0)
	{
		fclose(savefile);
		SET_ERROR_PARAM(r_error, TOXFILE_ERR_OPEN_FILE_SIZE);
		return NULL;
	}

	// Buffer
	size_t savesize = (size_t)_savesize;
	uint8_t *savedata = (uint8_t*)malloc(savesize * sizeof(uint8_t));
	if(savedata == NULL)
	{
		fclose(savefile);
		SET_ERROR_PARAM(r_error, TOXFILE_ERR_OPEN_MALLOC);
		return NULL;
	}
	memset(savedata, 0, savesize);

	// Read file
	size_t numread = fread(savedata, 1, savesize, savefile);
	fclose(savefile);

	// File size mismatch
	if(savesize != numread)
	{
		free(savedata);
		fprintf(stderr, "error reading toxfile, expected filesize != read filesize: %i != %i\n", savesize, numread);
		SET_ERROR_PARAM(r_error, TOXFILE_ERR_OPEN_FILE_READ);
		return NULL;
	}

	TOX_ERR_NEW error;
	Tox *tox = tox_new(NULL, savedata, savesize, &error);

#ifndef TOXFILE_NO_ENC
	// Prompt for password and try to use that to decrypt
	if(error == TOX_ERR_NEW_LOAD_ENCRYPTED)
	{
		// Get password
		char passphrase[PASS_MAX];
		memset(passphrase, 0, sizeof(passphrase));
		getpass("Tox savefile password: ", passphrase, sizeof(passphrase));

		// printf("[decrypt] password: %s, length: %i\n", passphrase, strlen(passphrase));

		// Try to decrypt savedata using inputted password
		size_t newdatasize = (savesize - TOX_PASS_ENCRYPTION_EXTRA_LENGTH);
		uint8_t *newdata = (uint8_t*)malloc(newdatasize * sizeof(uint8_t));
		if(newdata == NULL)
		{
			free(savedata);
			SET_ERROR_PARAM(r_error, TOXFILE_ERR_OPEN_MALLOC);
			return NULL;
		}

		TOX_ERR_DECRYPTION decrypt_error;
		tox_pass_decrypt(savedata, (size_t)savesize, (uint8_t*)passphrase, strlen(passphrase), newdata, &decrypt_error);

		if(decrypt_error == TOX_ERR_DECRYPTION_OK)
		{
			tox = tox_new(NULL, newdata, newdatasize, &error);
			free(newdata);
			if(error != TOX_ERR_NEW_OK)
			{
				free(savedata);
				SET_ERROR_PARAM(r_error, TOXFILE_ERR_OPEN_NEW);
				SET_ERROR_PARAM(r_new_error, error);
				return NULL;
			}

			SET_ERROR_PARAM(r_error, TOXFILE_ERR_OPEN_OK_ENCRYPTED);
		}
		else
		{
			SET_ERROR_PARAM(r_decrypt_error, decrypt_error);
			free(savedata);

			if(decrypt_error == TOX_ERR_DECRYPTION_FAILED)
			{
				fprintf(stderr, "Decryption failed\n");
				SET_ERROR_PARAM(r_error, TOXFILE_ERR_OPEN_DECRYPT_FAILED);
				return NULL;
			}
			else
			{
				fprintf(stderr, "Decryption error, TOX_ERR_DECRYPTION = %i\n", decrypt_error);
				SET_ERROR_PARAM(r_error, TOXFILE_ERR_OPEN_DECRYPT_ERROR);
				return NULL;
			}
		}

		error = TOX_ERR_NEW_OK;
	}
#endif

	free(savedata);

	if(error != TOX_ERR_NEW_OK)
	{
		fprintf(stderr, "tox_new error: %i\n", error);
		SET_ERROR_PARAM(r_error, TOXFILE_ERR_OPEN_NEW);
		SET_ERROR_PARAM(r_new_error, error);
		return NULL;
	}

	return tox;
}
