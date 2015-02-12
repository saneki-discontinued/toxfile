#ifndef __TOXFILE_H
#define __TOXFILE_H

#include <stdbool.h>
#include <stdint.h>

#define TOXFILE_OP_NONE 0
#define TOXFILE_OP_DECRYPT 1
#define TOXFILE_OP_ENCRYPT 2
#define TOXFILE_OP_HASH 3
#define TOXFILE_OP_NEW 4

// Exclusive print defines
#define TOXFILE_EXPRINT_NONE 0
#define TOXFILE_EXPRINT_PUBKEY 1
#define TOXFILE_EXPRINT_PRIVKEY 2
#define TOXFILE_EXPRINT_ADDRESS 3
#define TOXFILE_EXPRINT_NAME 4
#define TOXFILE_EXPRINT_STATUS 5

#define TOXFILE_HASH_MAX TOX_AVATAR_MAX_DATA_LENGTH

// Error constants
#define TOXFILE_ERR_FOPEN -100
#define TOXFILE_ERR_FWRITE -101
#define TOXFILE_ERR_FREAD -102
#define TOXFILE_ERR_ALREADY_ENC -110
#define TOXFILE_ERR_NOPATH -111
// Constants when tox.h functions fail
#define TOXFILE_ERR_ENCRYPTED_SAVE -200
#define TOXFILE_ERR_HASH -201

typedef struct __toxfile_args_t
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
	bool print_version;
} toxfile_args_t;

#define INIT_TOXFILE_ARGS { NULL, NULL, NULL, NULL, TOXFILE_OP_NONE, TOXFILE_EXPRINT_NONE, false, false, false, false }

// Prototypes
void toxfile_do(Tox *tox, toxfile_args_t *args);
int toxfile_hash(toxfile_args_t *args);
void toxfile_new(toxfile_args_t *args);
void toxfile_open(toxfile_args_t *args);
int toxfile_save(Tox *tox, const char *path);
void parse_arguments(toxfile_args_t *targs, int argc, char *argv[]);
void print_tox_fields(Tox *tox);
void print_help();
void print_version();

// Misc prototypes
void print_bytes(uint8_t *data, size_t size);

#ifndef TOXFILE_NO_ENC
int toxfile_encrypt(Tox *tox, toxfile_args_t *args);
int toxfile_decrypt(Tox *tox, toxfile_args_t *args);
int toxfile_save_enc(Tox *tox, const char *path, uint8_t *pass);
#endif

#endif // __TOXFILE_H
