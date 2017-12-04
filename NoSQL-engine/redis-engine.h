/*
 * =============================================================================
 *
 *       Filename:  redis-engine.h
 *
 *    Description:  redis storage engine
 *
 *        Created:  11/24/2012 03:49:23 PM
 *
 *         Author:  Fu Haiping (forhappy), haipingf@gmail.com
 *        Company:  ICT ( Institute Of Computing Technology, CAS )
 *
 * =============================================================================
 */

#ifndef __REDIS_ENGINE_H__
#define __REDIS_ENGINE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <hiredis/hiredis.h>

#include "../csas.h"

typedef struct engine_redis_config_s_ engine_redis_config_t;
typedef struct engine_redis_instance_s_ engine_redis_instance_t;
typedef struct engine_redis_s_ engine_redis_t;

/* 
 * redis config
 */
struct engine_redis_config_s_ {
    const char *ip;
    int        port;
};

/* 
 * redis context
 */
struct engine_redis_instance_s_ {
    redisContext *context;
};

/* 
 * redis
 */
struct engine_redis_s_ {
	/*
	 * engine base goes here, it MUST be a struct object, and NOT a struct pointer. 
	 */
	engine_base_t           base;
	engine_redis_config_t   *config;
	engine_redis_instance_t *instance;
};



extern engine_redis_t * engine_redis_init();

#endif /* __REDIS_ENGINE_H__ */
