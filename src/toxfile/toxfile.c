#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <linux/limits.h>
#include <getopt.h>
#include <termios.h>

#include <sys/stat.h>
#include <unistd.h>

#include <tox/tox.h>

#ifdef TOXSAVE_USE_ENC
#include <tox/toxencryptsave.h>
#endif

#include "toxfile.h"
#include "../file.h"

int main(int argc, char *argv[])
{
	toxsave_args_t targs = INIT_TOXSAVE_ARGS;
	parse_arguments(&targs, argc, argv);

	toxsave_open(&targs);

	return EXIT_SUCCESS;
}

void parse_arguments(toxsave_args_t *args, int argc, char *argv[])
{
	const char *argstr = ":t:aBdeH:nN:pPsh?";
	extern char *optarg;

	int option_index = 0;
	struct option long_options[] =
	{
		{ "toxfile",       required_argument, 0, 't' },
#ifdef TOXSAVE_USE_ENC
		{ "decrypt",       no_argument,       0, 'd' },
		{ "encrypt",       no_argument,       0, 'e' },
#endif
		{ "hash",          required_argument, 0, 'H' },
		{ "hash-write-bin",no_argument,       0, 'B' },
		{ "new",           required_argument, 0, 'N' },
		{ "print-address", no_argument,       0, 'a' },
		{ "print-name",    no_argument,       0, 'n' },
		{ "print-privkey", no_argument,       0, 'P' },
		{ "print-pubkey",  no_argument,       0, 'p' },
		{ "print-status",  no_argument,       0, 's' },
		{ "help",          no_argument,       0, 'h' },
		{ 0,               0,                 0,  0  }
	};

	int c;
	while((c = getopt_long(argc, argv, argstr, long_options, &option_index)) != -1)
	{
		switch(c)
		{
			case 't':
				args->savepath = optarg;
				break;

			case 'a':
				args->exclusive_print = TOXSAVE_EXPRINT_ADDRESS;
				break;

			case 'B':
				args->hash_print_bin = true;
				break;

#ifdef TOXSAVE_USE_ENC
			case 'd':
				args->operation = TOXSAVE_OP_DECRYPT;
				break;

			case 'e':
				args->operation = TOXSAVE_OP_ENCRYPT;
				break;
#endif
			case 'H':
				args->operation = TOXSAVE_OP_HASH;
				args->hash_path = optarg;
				break;

			case 'n':
				args->exclusive_print = TOXSAVE_EXPRINT_NAME;
				break;

			case 'N':
				args->operation = TOXSAVE_OP_NEW;
				args->new_path = optarg;
				break;

			case 'p':
				args->exclusive_print = TOXSAVE_EXPRINT_PUBKEY;
				break;

			case 'P':
				args->exclusive_print = TOXSAVE_EXPRINT_PRIVKEY;
				break;

			case 's':
				args->exclusive_print = TOXSAVE_EXPRINT_STATUS;
				break;

			case 'h':
			case '?':
				args->print_help = true;
				break;
		}
	}

	// Print help and exit
	if(args->print_help)
	{
		print_help();
		exit(EXIT_SUCCESS);
	}

	// Hashing a file doesn't need to open the tox save file
	if(args->operation == TOXSAVE_OP_HASH)
	{
		toxsave_hash(args);
		exit(EXIT_SUCCESS);
	}
	else if(args->operation == TOXSAVE_OP_NEW)
	{
		toxsave_new(args);
		exit(EXIT_SUCCESS);
	}
}

void print_help()
{
	printf("toxsave - utility for tox data files\n");
	printf("usage: toxsave [OPTIONS]\n");
	printf(" -t, --toxfile=PATH      path to tox save file\n");
	printf(" -a, --print-address     print tox address\n");
	printf(" -B, --hash-write-bin    write hash to stdout as binary\n");
#ifdef TOXSAVE_USE_ENC
	printf(" -d, --decrypt           decrypt tox save file\n");
	printf(" -e, --encrypt           encrypt tox save file\n");
#endif
	printf(" -H, --hash=PATH         hash a file, printing result to stdout\n");
	printf(" -n, --print-name        print tox name\n");
	printf(" -N, --new=PATH          create a new tox file\n");
	printf(" -p, --print-pubkey      print tox public key\n");
	printf(" -P, --print-privkey     print tox private key \n");
	printf(" -s, --print-status      print tox status message\n");
	printf(" -h, --help              print help/usage message (this)\n");
}

int toxsave_hash(toxsave_args_t *args)
{
	if(args->hash_path == NULL)
	{
		return TOXSAVE_ERR_NOPATH;
	}

	FILE *file = fopen(args->hash_path, "r");
	if(file == NULL)
	{
		fprintf(stderr, "Error hashing, can't open file\n");
		return TOXSAVE_ERR_FOPEN;
	}

	int64_t filesize = fsize(file);
	if(filesize > TOXSAVE_HASH_MAX)
	{
		filesize = TOXSAVE_HASH_MAX;
		printf("Filesize greater than max, hashing only first %i bytes\n", filesize);
	}

	uint8_t buffer[filesize], hash[TOX_HASH_LENGTH];

	size_t read = fread(buffer, 1, sizeof(buffer), file);
	if(read != sizeof(buffer))
	{
		fclose(file);
		fprintf(stderr, "Error hashing, read count mismatch\n");
		return TOXSAVE_ERR_FREAD;
	}

	int ret = tox_hash(hash, buffer, sizeof(buffer));
	if(ret != 0)
	{
		fclose(file);
		fprintf(stderr, "Error hashing, tox_hash failed\n");
		return TOXSAVE_ERR_HASH;
	}

	// Write to stdout as binary
	if(args->hash_print_bin)
	{
		fwrite(hash, 1, sizeof(hash), stdout);
	}
	else // Write to stdout as hex string
	{
		size_t hashstr_length = (sizeof(hash) * 2) + 1;
		uint8_t hashstr[hashstr_length];
		memset(hashstr, 0, hashstr_length);

		hexstr(hashstr, hash, sizeof(hash), false);
		printf("%s\n", hashstr);
	}

	return 0;
}

void toxsave_new(toxsave_args_t *args)
{
	// Check if file exists, prompt if the user wants to
	// overwrite an existing file
	struct stat st;
	if(stat(args->new_path, &st) == 0)
	{
		// If not regular file, just exit
		if(!S_ISREG(st.st_mode))
		{
			fprintf(stderr, "file exists and is not regular file\n");
			exit(EXIT_FAILURE);
		}

		int overwrite = toxsave_prompt_yn("File exists, overwrite? (y/N) ");
		if(overwrite < 0)
		{
			fprintf(stderr, "error prompting for input\n");
			exit(EXIT_FAILURE);
		}
		else if(overwrite != 0)
		{
			return;
		}
	}

	// Create new tox
	Tox *tox = tox_new(NULL);
	if(tox == NULL)
	{
		fprintf(stderr, "error creating new tox\n");
		exit(EXIT_FAILURE);
	}

	int result = toxsave_save(tox, args->new_path);
	if(result != 0) {
		exit(EXIT_FAILURE);
	}

	tox_kill(tox);
}

void toxsave_open(toxsave_args_t *args)
{
	char savepathbuf[PATH_MAX];
	char *savepath = args->savepath;

	// If no path passed in args, try to find path
	if(savepath == NULL)
	{
		toxsave_try_find_save_path(savepathbuf, sizeof(savepathbuf));

		// If still empty string (all 0x00s), couldn't find default file
		if(strlen(savepathbuf) == 0)
		{
			fprintf(stderr, "could not find file at default save path\n");
			exit(EXIT_FAILURE);
		}

		savepath = savepathbuf;
	}
	else
	{
		// Make sure path passed via args is at least a regular file
		struct stat st;
		if(stat(savepath, &st) != 0)
		{
			fprintf(stderr, "specified save file does not exist\n");
			exit(EXIT_FAILURE);
		}

		if(!S_ISREG(st.st_mode))
		{
			fprintf(stderr, "specified save file is not regular file\n");
			exit(EXIT_FAILURE);
		}
	}

	// Store opened path
	args->opened_path = savepath;

	// Try opening at savepath
	printf("Opening tox file at %s\n", savepath);
	FILE *savefile = fopen(savepath, "rb");

	if(savefile == NULL)
	{
		fprintf(stderr, "error opening tox savefile\n");
		exit(1);
	}

	Tox_Options opts;
	Tox *tox = tox_new(&opts);

	if(tox == NULL)
	{
		fclose(savefile);
		fprintf(stderr, "error initializing Tox struct (init_tox)\n");
		exit(1);
	}

	int64_t savesize = fsize(savefile);
	uint8_t *savedata = (uint8_t*)malloc(savesize * sizeof(uint8_t));
	memset(savedata, 0, savesize);

	int numread = fread(savedata, savesize, 1, savefile);
	fclose(savefile);

	int loadret = tox_load(tox, savedata, savesize);

	// If encrypted, decrypt if possible, otherwise error
	if(loadret == 1) {
#ifdef TOXSAVE_USE_ENC
		args->was_encrypted = true;

		char passphrase[PASS_MAX];
		toxsave_getpass("Tox savefile password: ", passphrase, sizeof(passphrase));

		loadret = tox_encrypted_load(tox, savedata, savesize, passphrase, strlen(passphrase));
#else
		free(savedata);
		fprintf(stderr, "Tox savefile is encrypted, aborting\n");
		exit(1);
#endif
	}

	if(loadret != 0)
	{
		free(savedata);
		fprintf(stderr, "error loading tox save file\n");
		exit(1);
	}

	printf("Successfully loaded tox save\n");

	toxsave_do(tox, args);

	tox_kill(tox);
}

#ifdef TOXSAVE_USE_ENC
// Decrypt a file based on args
int toxsave_decrypt(Tox *tox, toxsave_args_t *args)
{
	if(!args->was_encrypted)
	{
		fprintf(stderr, "Tox save file is already unencrypted\n");
		return TOXSAVE_ERR_ALREADY_ENC;
	}

	uint32_t size = tox_size(tox);
	uint8_t *savedata = (uint8_t*)malloc(size * sizeof(uint8_t));
	memset(savedata, 0, size);
	tox_save(tox, savedata);

	FILE *file = fopen(args->opened_path, "w");
	if(file == NULL)
	{
		free(savedata);
		fprintf(stderr, "error opening file to save\n");
		return TOXSAVE_ERR_FOPEN;
	}

	size_t written = fwrite(savedata, 1, size, file);

	fclose(file);
	free(savedata);

	if(written != size)
	{
		fprintf(stderr, "count mismatch when saving file\n");
		return TOXSAVE_ERR_FWRITE;
	}

	return 0;
}

// Encrypt a file based on args
int toxsave_encrypt(Tox *tox, toxsave_args_t *args)
{
	uint8_t passphrase[PASS_MAX];
	toxsave_getpass("Encrypt with password: ", passphrase, sizeof(passphrase));

	return toxsave_save_enc(tox, args->opened_path, passphrase);
}
#endif // TOXSAVE_USE_ENC

int toxsave_save(Tox *tox, const char *path)
{
	FILE *file = fopen(path, "wb");
	if(file == NULL)
	{
		fprintf(stderr, "error opening file\n");
		return TOXSAVE_ERR_FOPEN;
	}

	// Save tox to buffer
	uint32_t size = tox_size(tox);
	uint8_t data[size];
	tox_save(tox, data);

	int64_t written = fwrite(data, 1, size, file);
	fclose(file);

	if(written != size)
	{
		fprintf(stderr, "count mismatch while saving file\n");
		return TOXSAVE_ERR_FWRITE;
	}

	return 0;
}

#ifdef TOXSAVE_USE_ENC
int toxsave_save_enc(Tox *tox, const char *path, uint8_t *pass)
{
	FILE *file = fopen(path, "wb");
	if(file == NULL)
	{
		fprintf(stderr, "error opening file\n");
		return TOXSAVE_ERR_FOPEN;
	}

	uint32_t size = tox_encrypted_size(tox);
	uint8_t data[size];
	int success = tox_encrypted_save(tox, data, pass, strlen(pass));
	if(success != 0)
	{
		fprintf(stderr, "error using tox_encrypted_save");
		return TOXSAVE_ERR_ENCRYPTED_SAVE;
	}

	int written = fwrite(data, 1, size, file);
	fclose(file);

	if(written != size)
	{
		fprintf(stderr, "count mismatch while saving file\n");
		return TOXSAVE_ERR_FWRITE;
	}

	return 0;
}
#endif

void toxsave_do(Tox *tox, toxsave_args_t *args)
{
	// If operation
	if(args->operation != TOXSAVE_OP_NONE)
	{
		switch(args->operation)
		{
#ifdef TOXSAVE_USE_ENC
			case TOXSAVE_OP_DECRYPT:
				{
					toxsave_decrypt(tox, args);
					break;
				}
			case TOXSAVE_OP_ENCRYPT:
				{
					toxsave_encrypt(tox, args);
					break;
				}
#endif
		}
	}

	// If printing an exclusive value
	if(args->exclusive_print != TOXSAVE_EXPRINT_NONE)
	{
		switch(args->exclusive_print)
		{
			// Print only client address
			case TOXSAVE_EXPRINT_ADDRESS:
				{
					uint8_t tox_addr[TOX_FRIEND_ADDRESS_SIZE];
					tox_get_address(tox, tox_addr);
					print_bytes(tox_addr, sizeof(tox_addr));
					printf("\n");
				}
				break;

			// Print only name
			case TOXSAVE_EXPRINT_NAME:
				{
					uint8_t tox_name[TOX_MAX_NAME_LENGTH];
					memset(tox_name, 0, sizeof(tox_name));

					tox_get_self_name(tox, tox_name);
					printf("%s\n", tox_name);
				}
				break;

			// Print only the public key
			case TOXSAVE_EXPRINT_PUBKEY:
				{
					uint8_t tox_pub_key[32];
					tox_get_keys(tox, tox_pub_key, NULL);
					print_bytes(tox_pub_key, sizeof(tox_pub_key));
					printf("\n");
				}
				break;

			// Print only private key
			case TOXSAVE_EXPRINT_PRIVKEY:
				{
					uint8_t tox_priv_key[32];
					tox_get_keys(tox, NULL, tox_priv_key);
					print_bytes(tox_priv_key, sizeof(tox_priv_key));
					printf("\n");
				}
				break;

			// Print only status message
			case TOXSAVE_EXPRINT_STATUS:
				{
					uint8_t tox_status[TOX_MAX_STATUSMESSAGE_LENGTH];
					memset(tox_status, 0, sizeof(tox_status));

					tox_get_self_status_message(tox, tox_status, TOX_MAX_STATUSMESSAGE_LENGTH);
					printf("%s\n", tox_status);
				}
				break;
		}
	}
	else if(args->exclusive_print == TOXSAVE_EXPRINT_NONE
			&& args->operation == TOXSAVE_OP_NONE)
	{
		print_tox_fields(tox);
	}
}

void toxsave_try_find_save_path(char *dest, size_t destlen)
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
	joinpath(temp, homepath, TOXSAVE_NIX_HPATH_VENOM, sizeof(temp));
	if(stat(temp, &st) == 0 && S_ISREG(st.st_mode))
	{
		strncpy(dest, temp, destlen);
		return;
	}

	memset(temp, 0, sizeof(temp));

	// Check default uTox path for current user
	joinpath(temp, homepath, TOXSAVE_NIX_HPATH_UTOX, sizeof(temp));
	if(stat(temp, &st) == 0 && S_ISREG(st.st_mode))
	{
		strncpy(dest, temp, destlen);
		return;
	}

#endif
}

void print_tox_fields(Tox *tox)
{
	// --- Basic --- //

	uint8_t tox_name[TOX_MAX_NAME_LENGTH];
	uint8_t tox_status[TOX_MAX_STATUSMESSAGE_LENGTH];
	uint8_t tox_addr[TOX_FRIEND_ADDRESS_SIZE];

	memset(tox_name, 0, sizeof(tox_name));
	memset(tox_status, 0, sizeof(tox_status));

	tox_get_self_name(tox, tox_name);
	tox_get_self_status_message(tox, tox_status, TOX_MAX_STATUSMESSAGE_LENGTH);
	tox_get_address(tox, tox_addr);

	printf("Basic Info:\n");

	printf(" Address:    "); print_bytes(tox_addr, sizeof(tox_addr)); printf("\n");
	printf(" Tox Name:   %s\n", tox_name);
	printf(" Tox Status: %s\n", tox_status);

	// --- Crypto --- //

	uint8_t tox_pub_key[32], tox_priv_key[32];
	tox_get_keys(tox, tox_pub_key, tox_priv_key);

	printf("Crypto Info:\n");
	printf(" Public key:  "); print_bytes(tox_pub_key, sizeof(tox_pub_key)); printf("\n");
	printf(" Private key: "); print_bytes(tox_priv_key, sizeof(tox_priv_key)); printf("\n");
}

void print_bytes(uint8_t *data, size_t size)
{
	for(size_t i = 0; i < size; i++)
		printf("%02X", data[i]);
}

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

int toxsave_getpass(const char *text, char *out, size_t outlen)
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
int toxsave_prompt_yn(const char *message)
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

// length of dest should be exactly twice the length of src (or src_length * 2)
int hexstr(uint8_t *dest, uint8_t *src, size_t src_length, bool uppercase)
{
	char *format;
	if(uppercase) format = "%02X";
	else format = "%02x";

	size_t i;
	uint8_t c[2];

	for(i = 0; i < src_length; i++)
	{
		sprintf(c, format, src[i]);

		dest[(i*2)] = c[0];
		dest[(i*2)+1] = c[1];
	}

	return 0;
}
