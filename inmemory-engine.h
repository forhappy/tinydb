/*
 * =============================================================================
 *
 *       Filename:  inmemory-engine.h
 *
 *    Description:  inmemory storage engine
 *
 *        Created:  11/24/2012 03:49:23 PM
 *
 *         Author:  Fu Haiping (forhappy), haipingf@gmail.com
 *        Company:  ICT ( Institute Of Computing Technology, CAS )
 *
 * =============================================================================
 */

#ifndef INMEMORY_ENGINE_H
#define INMEMORY_ENGINE_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "uthash.h"
#include "csas.h"

typedef struct engine_inmemory_s_ engine_inmemory_t;
typedef struct inmemory_instance_s_ inmemory_instance_t;

struct inmemory_instance_s_ {
	char *key;
	char *value;
	UT_hash_handle hh;
};

struct engine_inmemory_s_ {
	/* engine base goes here, it MUST be a struct object,
	 * and NOT a struct pointer. 
	 * */
	engine_base_t base;

	inmemory_instance_t *instance;
};

extern engine_inmemory_t * engine_inmemory_init();

#endif /* INMEMORY_ENGINE_H */
