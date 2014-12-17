#include <stdint.h>
#include <jansson.h>
#include <tox/tox.h>
#include "toxdump.h"
#include "toxdump_json.h"
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

int toxdump_perform_json(Tox *tox, FILE *file)
{
	json_t *j_root = json_object();

	// Tox address
	uint8_t t_address_buffer[TOX_FRIEND_ADDRESS_SIZE];
	tox_get_address(tox, t_address_buffer);
	uint8_t t_address[TOX_FRIEND_ADDRESS_SIZE * 2];
	to_hex(t_address, t_address_buffer, sizeof(t_address_buffer), 0);

	// Tox name
	int t_name_size = tox_get_self_name_size(tox); // Assume success for now
	uint8_t t_name[t_name_size];
	tox_get_self_name(tox, t_name);

	// Tox status message
	int t_status_msg_size = tox_get_self_status_message_size(tox); // Assume success for now
	uint8_t t_status_msg[t_status_msg_size];
	tox_get_self_status_message(tox, t_status_msg, t_status_msg_size);

	// Initialize json_t variables
	json_t *j_str_name = json_stringn((char*)t_name, sizeof(t_name));
	json_t *j_str_status_msg = json_stringn((char*)t_status_msg, sizeof(t_status_msg));
	json_t *j_str_address = json_stringn((char*)t_address, sizeof(t_address));

	// Set fields to root variable
	json_object_set(j_root, "name", j_str_name);
	json_object_set(j_root, "status_message", j_str_status_msg);
	json_object_set(j_root, "address", j_str_address);

	// Write to file
	int success = json_dumpf(j_root, file, 0);

	// Free json_t variables
	free(j_str_name);
	free(j_str_status_msg);
	free(j_str_address);
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
