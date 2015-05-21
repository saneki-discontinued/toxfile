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
#include <stdint.h>
#include <string.h>
#include <linux/limits.h>
#include <getopt.h>

#include <sys/stat.h>
#include <unistd.h>

#include <tox/tox.h>

#ifndef TOXFILE_NO_ENC
#include <tox/toxencryptsave.h>
#endif

#include <libsy.h>

#include "toxfile.h"
#include "../toxfile_util.h"
#include "../io.h"
#include "../path.h"
#include "../version.h"

int main(int argc, char *argv[])
{
	toxfile_args_t targs = INIT_TOXFILE_ARGS;
	parse_arguments(&targs, argc, argv);

	toxfile_open_with(&targs);

	return EXIT_SUCCESS;
}

void parse_arguments(toxfile_args_t *args, int argc, char *argv[])
{
	const char *argstr = ":t:aBdeH:kmN:sxh?v";
	extern char *optarg;

	int option_index = 0, index;
	struct option long_options[] =
	{
#ifndef TOXFILE_NO_ENC
		{ "decrypt",       no_argument,       0, 'd' },
		{ "encrypt",       no_argument,       0, 'e' },
#endif
		{ "hash",          required_argument, 0, 'H' },
		{ "hash-write-bin",no_argument,       0, 'B' },
		{ "new",           required_argument, 0, 'N' },
		{ "print-address", no_argument,       0, 'a' },
		{ "print-name",    no_argument,       0, 'm' },
		{ "print-privkey", no_argument,       0, 'x' },
		{ "print-pubkey",  no_argument,       0, 'k' },
		{ "print-status-message", no_argument, 0, 's' },
		{ "print-status",  no_argument,       0, 'u' },
		{ "set-address",   required_argument, 0, 'A' },
		{ "set-name",      required_argument, 0, 'M' },
		{ "set-privkey",   required_argument, 0, 'X' },
		{ "set-pubkey",    required_argument, 0, 'K' },
		{ "set-status",    required_argument, 0, 'U' },
		{ "set-status-message", required_argument, 0, 'S' },
		{ "help",          no_argument,       0, 'h' },
		{ "version",       no_argument,       0, 'v' },
		{ 0,               0,                 0,  0  }
	};

	int c;
	while((c = getopt_long(argc, argv, argstr, long_options, &option_index)) != -1)
	{
		switch(c)
		{
			case 'a':
				args->exclusive_print = TOXFILE_EXPRINT_ADDRESS;
				break;

			case 'B':
				args->hash_print_bin = true;
				break;

#ifndef TOXFILE_NO_ENC
			case 'd':
				args->operation = TOXFILE_OP_DECRYPT;
				break;

			case 'e':
				args->operation = TOXFILE_OP_ENCRYPT;
				break;
#endif
			case 'H':
				args->operation = TOXFILE_OP_HASH;
				args->hash_path = optarg;
				break;

			case 'm':
				args->exclusive_print = TOXFILE_EXPRINT_NAME;
				break;

			case 'N':
				args->operation = TOXFILE_OP_NEW;
				args->new_path = optarg;
				break;

			case 'k':
				args->exclusive_print = TOXFILE_EXPRINT_PUBKEY;
				break;

			case 'x':
				args->exclusive_print = TOXFILE_EXPRINT_PRIVKEY;
				break;

			case 's':
				args->exclusive_print = TOXFILE_EXPRINT_STATUS;
				break;

			case 'h':
			case '?':
				args->print_help = true;
				break;

			case 'v':
				args->print_version = true;
				break;
		}
	}

	for(index = optind; index < argc; index++)
	{
		args->savepath = argv[index];
		break;
	}

	// Print help and exit
	if(args->print_help)
	{
		print_help();
		exit(EXIT_SUCCESS);
	}

	if(args->print_version)
	{
		print_version();
		exit(EXIT_SUCCESS);
	}

	// Hashing a file doesn't need to open the tox save file
	if(args->operation == TOXFILE_OP_HASH)
	{
		toxfile_hash(args);
		exit(EXIT_SUCCESS);
	}
	else if(args->operation == TOXFILE_OP_NEW)
	{
		toxfile_new(args);
		exit(EXIT_SUCCESS);
	}
}

void print_help()
{
	printf("toxfile - general purpose utility for tox files\n");
	printf("usage: toxfile [options] <file>\n");
	printf(" -a, --print-address     print tox address\n");
	printf(" -B, --hash-write-bin    write hash to stdout as binary\n");
#ifndef TOXFILE_NO_ENC
	printf(" -d, --decrypt           decrypt tox save file\n");
	printf(" -e, --encrypt           encrypt tox save file\n");
#endif
	printf(" -H, --hash=PATH         hash a file, printing result to stdout\n");
	printf(" -k, --print-pubkey      print tox public key\n");
	printf(" -m, --print-name        print tox name\n");
	printf(" -N, --new=PATH          create a new tox file\n");
	printf(" -s, --print-status-message    print tox status message\n");
	printf(" -x, --print-privkey     print tox private key \n");
	printf(" -h, -?, --help          print help/usage message (this)\n");
	printf(" -v, --version           print toxfile version\n");
}

void print_version()
{
	printf("toxfile v%s\n", TOXFILE_PROJ_VERSION);
}

void toxfile_open_with(toxfile_args_t *args)
{
	TOXFILE_ERR_OPEN error;
	Tox *tox = toxfile_open(args->savepath, &error);
	if(error != TOXFILE_ERR_OPEN_OK && error != TOXFILE_ERR_OPEN_OK_ENCRYPTED)
	{
		fprintf(stderr, "toxfile_open error: %i\n", error);
		exit(EXIT_FAILURE);
	}

	args->was_encrypted = (error == TOXFILE_ERR_OPEN_OK_ENCRYPTED);

	// Store opened path
	args->opened_path = args->savepath;

	toxfile_do(tox, args);
	tox_kill(tox);
}

int toxfile_hash(toxfile_args_t *args)
{
	if(args->hash_path == NULL)
	{
		return TOXFILE_ERR_NOPATH;
	}

	FILE *file = fopen(args->hash_path, "r");
	if(file == NULL)
	{
		fprintf(stderr, "Error hashing, can't open file\n");
		return TOXFILE_ERR_FOPEN;
	}

	int64_t filesize = fsize(file);
	if(filesize > TOX_HASH_LENGTH)
	{
		filesize = TOX_HASH_LENGTH;
		printf("Filesize greater than max, hashing only first %i bytes\n", filesize);
	}

	uint8_t buffer[filesize], hash[TOX_HASH_LENGTH];

	size_t read = fread(buffer, 1, sizeof(buffer), file);
	if(read != sizeof(buffer))
	{
		fclose(file);
		fprintf(stderr, "Error hashing, read count mismatch\n");
		return TOXFILE_ERR_FREAD;
	}

	int ret = tox_hash(hash, buffer, sizeof(buffer));
	if(ret != 0)
	{
		fclose(file);
		fprintf(stderr, "Error hashing, tox_hash failed\n");
		return TOXFILE_ERR_HASH;
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

		hexx(hashstr, hash, sizeof(hash), 0);
		printf("%s\n", hashstr);
	}

	return 0;
}

void toxfile_new(toxfile_args_t *args)
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

		int overwrite = prompt_yn("File exists, overwrite? (y/N) ");
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
	TOX_ERR_NEW error;
	Tox *tox = tox_new(NULL, NULL, 0, &error);
	if(error != TOX_ERR_NEW_OK)
	{
		fprintf(stderr, "tox_new error: %i\n", error);
		exit(EXIT_FAILURE);
	}

	int result = toxfile_save(tox, args->new_path);
	if(result != 0) {
		exit(EXIT_FAILURE);
	}

	tox_kill(tox);
}

#ifndef TOXFILE_NO_ENC
// Decrypt a file based on args
int toxfile_decrypt(Tox *tox, toxfile_args_t *args)
{
	if(!args->was_encrypted)
	{
		fprintf(stderr, "Tox save file is already unencrypted\n");
		return TOXFILE_ERR_ALREADY_ENC;
	}

	uint32_t size = tox_get_savedata_size(tox);
	uint8_t *savedata = (uint8_t*)malloc(size * sizeof(uint8_t));
	memset(savedata, 0, size);
	tox_get_savedata(tox, savedata);

	FILE *file = fopen(args->opened_path, "w");
	if(file == NULL)
	{
		free(savedata);
		fprintf(stderr, "error opening file to save\n");
		return TOXFILE_ERR_FOPEN;
	}

	size_t written = fwrite(savedata, 1, size, file);

	fclose(file);
	free(savedata);

	if(written != size)
	{
		fprintf(stderr, "count mismatch when saving file\n");
		return TOXFILE_ERR_FWRITE;
	}

	return 0;
}

// Encrypt a file based on args
int toxfile_encrypt(Tox *tox, toxfile_args_t *args)
{
	uint8_t passphrase[PASS_MAX];
	getpass("Encrypt with password: ", passphrase, sizeof(passphrase));

	printf("Encrypting with password: %s\n", passphrase);
	return toxfile_save_enc(tox, args->opened_path, passphrase);
}
#endif // ! TOXFILE_NO_ENC

int toxfile_save(Tox *tox, const char *path)
{
	FILE *file = fopen(path, "wb");
	if(file == NULL)
	{
		fprintf(stderr, "error opening file\n");
		return TOXFILE_ERR_FOPEN;
	}

	// Save tox to buffer
	uint32_t size = tox_get_savedata_size(tox);
	uint8_t data[size];
	tox_get_savedata(tox, data);

	int64_t written = fwrite(data, 1, size, file);
	fclose(file);

	if(written != size)
	{
		fprintf(stderr, "count mismatch while saving file\n");
		return TOXFILE_ERR_FWRITE;
	}

	return 0;
}

#ifndef TOXFILE_NO_ENC
int toxfile_save_enc(Tox *tox, const char *path, uint8_t *pass)
{
	FILE *file = fopen(path, "wb");
	if(file == NULL)
	{
		fprintf(stderr, "error opening file\n");
		return TOXFILE_ERR_FOPEN;
	}

	uint32_t data_size = tox_get_savedata_size(tox);
	uint8_t *data = (uint8_t*)malloc(data_size * sizeof(uint8_t));
	tox_get_savedata(tox, data);

	size_t encdata_size = (data_size + TOX_PASS_ENCRYPTION_EXTRA_LENGTH);
	uint8_t *encdata = (uint8_t*)malloc(encdata_size * sizeof(uint8_t));
	TOX_ERR_ENCRYPTION encrypt_error;
	tox_pass_encrypt(data, data_size, pass, strlen(pass), encdata, &encrypt_error);
	free(data);

	if(encrypt_error != TOX_ERR_ENCRYPTION_OK)
	{
		fclose(file);
		free(encdata);
		fprintf(stderr, "tox_pass_encrypt error: %i\n", encrypt_error);
		return TOXFILE_ERR_ENCRYPTED_SAVE;
	}

	size_t written = fwrite(encdata, 1, encdata_size, file);
	fclose(file);
	free(encdata);

	if(written != encdata_size)
	{
		fprintf(stderr, "count mismatch while saving file\n");
		return TOXFILE_ERR_FWRITE;
	}

	return 0;
}
#endif

void toxfile_do(Tox *tox, toxfile_args_t *args)
{
	// If operation
	if(args->operation != TOXFILE_OP_NONE)
	{
		switch(args->operation)
		{
#ifndef TOXFILE_NO_ENC
			case TOXFILE_OP_DECRYPT:
				{
					toxfile_decrypt(tox, args);
					break;
				}
			case TOXFILE_OP_ENCRYPT:
				{
					toxfile_encrypt(tox, args);
					break;
				}
#endif
		}
	}

	// If printing an exclusive value
	if(args->exclusive_print != TOXFILE_EXPRINT_NONE)
	{
		switch(args->exclusive_print)
		{
			// Print only client address
			case TOXFILE_EXPRINT_ADDRESS:
				{
					uint8_t tox_addr[TOX_ADDRESS_SIZE];
					tox_self_get_address(tox, tox_addr);
					print_bytes(tox_addr, sizeof(tox_addr));
					printf("\n");
				}
				break;

			// Print only name
			case TOXFILE_EXPRINT_NAME:
				{
					uint8_t tox_name[TOX_MAX_NAME_LENGTH];
					memset(tox_name, 0, sizeof(tox_name));

					tox_self_get_name(tox, tox_name);
					printf("%s\n", tox_name);
				}
				break;

			// Print only the public key
			case TOXFILE_EXPRINT_PUBKEY:
				{
					uint8_t tox_pub_key[TOX_PUBLIC_KEY_SIZE];
					tox_self_get_public_key(tox, tox_pub_key);
					print_bytes(tox_pub_key, sizeof(tox_pub_key));
					printf("\n");
				}
				break;

			// Print only private key
			case TOXFILE_EXPRINT_PRIVKEY:
				{
					uint8_t tox_priv_key[TOX_SECRET_KEY_SIZE];
					tox_self_get_secret_key(tox, tox_priv_key);
					print_bytes(tox_priv_key, sizeof(tox_priv_key));
					printf("\n");
				}
				break;

			// Print only status message
			case TOXFILE_EXPRINT_STATUS:
				{
					uint8_t tox_status[TOX_MAX_STATUS_MESSAGE_LENGTH];
					memset(tox_status, 0, sizeof(tox_status));

					tox_self_get_status_message(tox, tox_status);
					printf("%s\n", tox_status);
				}
				break;
		}
	}
	else if(args->exclusive_print == TOXFILE_EXPRINT_NONE
			&& args->operation == TOXFILE_OP_NONE)
	{
		print_tox_fields(tox);
	}
}

void print_tox_fields(Tox *tox)
{
	// --- Basic --- //

	uint8_t tox_name[TOX_MAX_NAME_LENGTH];
	uint8_t tox_status[TOX_MAX_STATUS_MESSAGE_LENGTH];
	uint8_t tox_addr[TOX_ADDRESS_SIZE];

	memset(tox_name, 0, sizeof(tox_name));
	memset(tox_status, 0, sizeof(tox_status));

	tox_self_get_name(tox, tox_name);
	tox_self_get_status_message(tox, tox_status);
	tox_self_get_address(tox, tox_addr);

	printf("Basic Info:\n");

	printf(" Address:    "); print_bytes(tox_addr, sizeof(tox_addr)); printf("\n");
	printf(" Tox Name:   %s\n", tox_name);
	printf(" Tox Status: %s\n", tox_status);

	// --- Crypto --- //

	uint8_t tox_pub_key[TOX_PUBLIC_KEY_SIZE];
	tox_self_get_public_key(tox, tox_pub_key);

	printf("Crypto Info:\n");
	printf(" Public key:  "); print_bytes(tox_pub_key, sizeof(tox_pub_key)); printf("\n");
}

void print_bytes(uint8_t *data, size_t size)
{
	for(size_t i = 0; i < size; i++)
		printf("%02X", data[i]);
}
