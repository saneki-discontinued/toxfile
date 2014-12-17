#include <stdint.h>
#include <jansson.h>
#include <tox/tox.h>
#include "toxdump.h"
#include "toxdump_json.h"
#include "toxfile_state.h"
#include "hex.h"

void *jansson_handle = NULL;

void toxdump_create_json_module(toxdump_module *module)
{
	//jansson_handle = dlopen("libjansson.so", RTLD_LAZY);
	//if(!jansson_handle)
	//{
	//	return;
	//}

	// Todo: Set global variable function pointers

	module->func = toxdump_perform_json;
}

int toxdump_perform_json(toxfile_state_t *state, FILE *file)
{
	json_t *j_root = json_object();

	// Binary -> Hex strings
	uint8_t t_address_hex[sizeof(state->address) * 2];
	uint8_t t_public_key_hex[sizeof(state->public_key) * 2];
	to_hex(t_address_hex, (char*)state->address, sizeof(state->address), 0);
	to_hex(t_public_key_hex, (char*)state->address, sizeof(state->address), 0);

	// Initialize json_t variables
	json_t *j_str_name = json_stringn((char*)state->name, state->name_len);
	json_t *j_str_status_msg = json_stringn((char*)state->status_message, state->status_message_len);
	json_t *j_str_address = json_stringn((char*)t_address_hex, sizeof(t_address_hex));
	json_t *j_int_status = json_integer(state->status);
	json_t *j_str_public_key = json_stringn((char*)t_public_key_hex, sizeof(t_public_key_hex));
	json_t *j_str_private_key = NULL;

	// Set fields to root variable
	json_object_set(j_root, "address", j_str_address);
	json_object_set(j_root, "name", j_str_name);
	json_object_set(j_root, "public_key", j_str_public_key);
	json_object_set(j_root, "status", j_int_status);
	json_object_set(j_root, "status_message", j_str_status_msg);

	if(state->has_private_key == 1)
	{
		uint8_t t_private_key_hex[sizeof(state->private_key) * 2];
		to_hex(t_private_key_hex, state->private_key, sizeof(state->private_key), 0);
		j_str_private_key = json_stringn((char*)t_private_key_hex, sizeof(t_private_key_hex));
		json_object_set(j_root, "private_key", j_str_private_key);
	}

	// Write to file
	int success = json_dumpf(j_root, file, JSON_SORT_KEYS);

	// Free json_t variables
	free(j_str_name);
	free(j_str_status_msg);
	free(j_str_address);
	free(j_int_status);
	free(j_str_public_key);

	if(j_str_private_key != NULL)
	{
		free(j_str_private_key);
	}

	free(j_root);

	return success;
}

void toxdump_teardown_json_module()
{
	//if(jansson_handle)
	//{
	//	dlclose(jansson_handle);
	//}
}
