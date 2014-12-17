#ifndef TOXFILE_STATE_H
#define TOXFILE_STATE_H

#include <stdint.h>
#include <tox/tox.h>

#define TOXFILE_LOAD_PRIVKEY (1<<0)

typedef struct __toxfile_state_t
{
	uint8_t address[TOX_FRIEND_ADDRESS_SIZE];
	uint8_t has_private_key;
	uint8_t name[TOX_MAX_NAME_LENGTH];
	int name_len;
	uint8_t public_key[TOX_CLIENT_ID_SIZE];
	uint8_t private_key[TOX_CLIENT_ID_SIZE];
	uint8_t status;
	uint8_t status_message[TOX_MAX_STATUSMESSAGE_LENGTH];
	int status_message_len;
} toxfile_state_t;

int toxfile_load_state(Tox *tox, toxfile_state_t *state, size_t flags);

#endif // TOXFILE_STATE_H
