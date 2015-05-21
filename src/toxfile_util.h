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

#ifndef TOXFILE_UTIL_H
#define TOXFILE_UTIL_H

#include <tox/tox.h>

#ifndef TOXFILE_NO_ENC
#include <tox/toxencryptsave.h>
#else
typedef int TOX_ERR_DECRYPTION;
#endif

typedef enum TOXFILE_ERR_OPEN
{
	TOXFILE_ERR_OPEN_OK,
	TOXFILE_ERR_OPEN_OK_ENCRYPTED,
	TOXFILE_ERR_OPEN_MALLOC,
	TOXFILE_ERR_OPEN_NULL,
	TOXFILE_ERR_OPEN_FILE,
	TOXFILE_ERR_OPEN_FILE_NOT_FOUND,
	TOXFILE_ERR_OPEN_FILE_NOT_REGULAR,
	TOXFILE_ERR_OPEN_FILE_READ,
	TOXFILE_ERR_OPEN_FILE_SIZE,
	TOXFILE_ERR_OPEN_NEW,
	TOXFILE_ERR_OPEN_NO_DEFAULT,
	TOXFILE_ERR_OPEN_DECRYPT_FAILED,
	TOXFILE_ERR_OPEN_DECRYPT_ERROR
} TOXFILE_ERR_OPEN;

Tox * toxfile_open(const char *savepath, TOXFILE_ERR_OPEN *error);
Tox * toxfile_open_ex(const char *savepath, TOXFILE_ERR_OPEN *r_error, TOX_ERR_NEW *r_new_error, TOX_ERR_DECRYPTION *r_decrypt_error);

#endif // TOXFILE_UTIL_H
