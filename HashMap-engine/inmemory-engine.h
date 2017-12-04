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

#ifndef __INMEMORY_ENGINE_H__
#define __INMEMORY_ENGINE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "uthash.h"
#include "../csas.h"

typedef struct engine_inmemory_config_s_ engine_inmemory_config_t;
typedef struct engine_inmemory_instance_s_ engine_inmemory_instance_t;
typedef struct engine_inmemory_s_ engine_inmemory_t;

/* 
 * inmemory config
 */
struct engine_inmemory_config_s_ {
};

/* 
 * inmemory instance
 */
struct engine_inmemory_instance_s_ {
	char           *key;
	char           *value;
	UT_hash_handle hh;
};

/* 
 * inmemory
 */
struct engine_inmemory_s_ {
	/* 
	 * engine base goes here, it MUST be a struct object, and NOT a struct pointer. 
	 */
	engine_base_t              base;
	engine_inmemory_config_t   *config;
	engine_inmemory_instance_t *instance;
};

extern engine_inmemory_t * engine_inmemory_init();

#endif /* __INMEMORY_ENGINE_H__ */
