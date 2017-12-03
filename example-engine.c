/*
 * =============================================================================
 *
 *       Filename:  example-engine.c
 *
 *    Description:  example storage engine.
 *
 *        Created:  11/24/2012 03:49:34 PM
 *
 *         Author:  Fu Haiping (forhappy), haipingf@gmail.com
 *        Company:  ICT ( Institute Of Computing Technology, CAS )
 *
 * =============================================================================
 */

#include "example-engine.h"

static int
put(engine_base_t *engine,
	const char *key,
	size_t key_len,
	const char *value,
	size_t value_len)
{
	printf("hook call to example engine put.\n");

	return 0;
}

static char *
get(engine_base_t *engine,
	const char *key,
	size_t key_len,
	size_t *value_len)
{
	printf("hook call to example engine get.\n");

	return NULL;
}

static int
delete(engine_base_t *engine,
	   const char *key,
	   size_t key_len)
{
	printf("hook call to example engine delete.\n");

	return 0;
}

engine_base_t *
engine_example_init(void)
{
	engine_base_t *engine = (engine_base_t *) malloc(sizeof(engine_base_t));
	engine_operation_t *engine_ops = (engine_operation_t *)malloc(sizeof(engine_operation_t));

	const char *engine_name = "example engine v0.1";
	size_t engine_name_len = strlen(engine_name);
	engine->name = malloc(sizeof(char) * (engine_name_len + 1));
	memset(engine->name, 0, (engine_name_len + 1));
	strncpy(engine->name, engine_name, engine_name_len);

	size_t version = 0x1;
	engine->version = version;

	engine_ops->put    = put;
	engine_ops->get    = get;
	engine_ops->delete = delete;

	engine->engine_ops = engine_ops;

	return engine;
}
