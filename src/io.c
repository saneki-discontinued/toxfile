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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/limits.h>
#include <termios.h>
#include <unistd.h>
#include "io.h"

int getpass(const char *text, char *out, size_t outlen)
{
	if(out == NULL || outlen == 0)
	{
		return -1;
	}

	memset(out, 0, outlen);

	struct termios oflags, nflags;

	tcgetattr(STDIN_FILENO, &oflags);
	nflags = oflags;
	nflags.c_lflag &= ~ECHO;
	nflags.c_lflag |= ECHONL;

	// Disable echo in console
	if(tcsetattr(STDIN_FILENO, TCSANOW, &nflags) != 0)
	{
		return -1;
	}

	if(text != NULL)
	{
		printf(text);
	}

	fgets(out, outlen, stdin);
	out[outlen - 1] = 0; // Null term

	// Restore console
	if(tcsetattr(STDIN_FILENO, TCSANOW, &oflags) != 0)
	{
		return -1;
	}

	return 0;
}

// Prompt for yes/no input
// Returns 0 on yes, 1 on no, a negative integer on error
int prompt_yn(const char *message)
{
	if(message != NULL)
	{
		printf(message);
	}

	int answer = fgetc(stdin);
	if(answer < 0)
	{
		return -1;
	}

	if(answer == 'y' || answer == 'Y') return 0;
	else return 1;
}
