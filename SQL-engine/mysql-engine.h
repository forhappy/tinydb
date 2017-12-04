/*
 * =============================================================================
 *
 *       Filename:  mysql-engine.h
 *
 *    Description:  mysql storage engine
 *
 *        Created:  11/24/2012 03:49:23 PM
 *
 *         Author:  Fu Haiping (forhappy), haipingf@gmail.com
 *        Company:  ICT ( Institute Of Computing Technology, CAS )
 *
 * =============================================================================
 */

#ifndef __MYSQL_ENGINE_H__
#define __MYSQL_ENGINE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../csas.h"

extern engine_base_t * engine_mysql_init();

#endif /* __MYSQL_ENGINE_H__ */
