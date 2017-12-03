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

#include "../csas.h"

extern engine_base_t * engine_redis_init();

#endif /* __REDIS_ENGINE_H__ */
