/*
 * =============================================================================
 *
 *       Filename:  memcached-engine.h
 *
 *    Description:  memcached storage engine
 *
 *        Created:  11/24/2012 03:49:23 PM
 *
 *         Author:  Fu Haiping (forhappy), haipingf@gmail.com
 *        Company:  ICT ( Institute Of Computing Technology, CAS )
 *
 * =============================================================================
 */

#ifndef __MEMCACHED_ENGINE_H__
#define __MEMCACHED_ENGINE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <libmemcached/memcached.h>

#include "../csas.h"

typedef struct engine_memcached_config_s_ engine_memcached_config_t;
typedef struct engine_memcached_instance_s_ engine_memcached_instance_t;
typedef struct engine_memcached_s_ engine_memcached_t;

/* 
 * memcached config
 */
struct engine_memcached_config_s_ {
	const char* ip;
	size_t      port;
};

/* 
 * memcached context
 */
struct engine_memcached_instance_s_ {
    memcached_st *context;
};

/* 
 * memcached
 */
struct engine_memcached_s_ {
	/*
	 * engine base goes here, it MUST be a struct object, and NOT a struct pointer. 
	 */
	engine_base_t               base;
	engine_memcached_config_t   *config;
	engine_memcached_instance_t *instance;
};



extern engine_memcached_t * engine_memcached_init();

#endif /* __MEMCACHED_ENGINE_H__ */
