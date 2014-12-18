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
