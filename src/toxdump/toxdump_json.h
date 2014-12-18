#ifndef TOXDUMP_JSON_H
#define TOXDUMP_JSON_H

#include "toxdump.h"
#include "../toxfile_state.h"

int toxdump_perform_json(toxfile_state_t *state, FILE *file, toxdump_args_t *args);

#endif
