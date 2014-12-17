#ifndef __TOXSAVE_H
#define __TOXSAVE_H

#include <stdbool.h>
#include <stdint.h>

#define TOXSAVE_OP_NONE 0
#define TOXSAVE_OP_DECRYPT 1
#define TOXSAVE_OP_ENCRYPT 2
#define TOXSAVE_OP_HASH 3
#define TOXSAVE_OP_NEW 4

// Exclusive print defines
#define TOXSAVE_EXPRINT_NONE 0
#define TOXSAVE_EXPRINT_PUBKEY 1
#define TOXSAVE_EXPRINT_PRIVKEY 2
#define TOXSAVE_EXPRINT_ADDRESS 3
#define TOXSAVE_EXPRINT_NAME 4
#define TOXSAVE_EXPRINT_STATUS 5

// Default paths to save file, relative to home (on *nix systems)
#define TOXSAVE_NIX_HPATH_VENOM ".config/tox/data"
#define TOXSAVE_NIX_HPATH_UTOX ".config/tox/tox_save"

#ifndef PASS_MAX
#define PASS_MAX 4096
#endif

#define TOXSAVE_HASH_MAX TOX_AVATAR_MAX_DATA_LENGTH

// Error constants
#define TOXSAVE_ERR_FOPEN -100
#define TOXSAVE_ERR_FWRITE -101
#define TOXSAVE_ERR_FREAD -102
#define TOXSAVE_ERR_ALREADY_ENC -110
#define TOXSAVE_ERR_NOPATH -111
// Constants when tox.h functions fail
#define TOXSAVE_ERR_ENCRYPTED_SAVE -200
#define TOXSAVE_ERR_HASH -201

typedef struct __toxsave_args_t
{
	char *savepath;      // Path to the savefile
	char *opened_path;   // Path to file after opened
	char *hash_path;     // Path to file to be hashed
	char *new_path;      // Path to new tox file
	int operation;
	int exclusive_print; // Which field to print exclusively
	bool was_encrypted;
	bool hash_print_bin;
	bool print_help;
} toxsave_args_t;

#define INIT_TOXSAVE_ARGS { NULL, NULL, NULL, NULL, TOXSAVE_OP_NONE, TOXSAVE_EXPRINT_NONE, false, false, false }

// Prototypes
void toxsave_do(Tox *tox, toxsave_args_t *args);
int toxsave_getpass(const char *text, char *out, size_t outlen);
int toxsave_hash(toxsave_args_t *args);
void toxsave_new(toxsave_args_t *args);
void toxsave_open(toxsave_args_t *args);
int toxsave_save(Tox *tox, const char *path);
int toxsave_prompt_yn(const char *message);
void toxsave_try_find_save_path(char *dest, size_t destlen);
void parse_arguments(toxsave_args_t *targs, int argc, char *argv[]);
void print_tox_fields(Tox *tox);
void print_help();

// Misc prototypes
int hexstr(uint8_t *dest, uint8_t *src, size_t src_length, bool uppercase);
int64_t fsize(FILE *file);
void joinpath(char *dest, const char *part1, const char *part2, uint32_t maxlen);
void print_bytes(uint8_t *data, size_t size);

#ifdef TOXSAVE_USE_ENC
int toxsave_encrypt(Tox *tox, toxsave_args_t *args);
int toxsave_decrypt(Tox *tox, toxsave_args_t *args);
int toxsave_save_enc(Tox *tox, const char *path, uint8_t *pass);
#endif

#endif
