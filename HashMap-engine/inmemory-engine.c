/*
 * =============================================================================
 *
 *       Filename:  inmemory-engine.c
 *
 *    Description:  inmemory storage engine.
 *
 *        Created:  11/24/2012 03:49:34 PM
 *
 *         Author:  Fu Haiping (forhappy), haipingf@gmail.com
 *        Company:  ICT ( Institute Of Computing Technology, CAS )
 *
 * =============================================================================
 */

#include "inmemory-engine.h"

static int
put(engine_base_t *engine,
	const char *key,
	size_t key_len,
	const char *value,
	size_t value_len)
{
	engine_inmemory_instance_t *tmp = NULL;
	engine_inmemory_t *engine_inmemory = (engine_inmemory_t *)engine;

	HASH_FIND_STR(engine_inmemory->instance, key, tmp);
	if (tmp == NULL) {
		tmp = (engine_inmemory_instance_t *)malloc(sizeof(engine_inmemory_instance_t));
		if (tmp == NULL) {
			fprintf(stderr, "put key-value pair failed due to out of memory.\n");

			return -1;
		}

		tmp->key = (char *)malloc(sizeof(char) * (key_len + 1));
		memset(tmp->key, 0, (key_len + 1));
		strncpy(tmp->key, key, key_len);

		tmp->value = (char *)malloc(sizeof(char) * (value_len + 1));
		memset(tmp->value, 0, (value_len + 1));
		strncpy(tmp->value, value, value_len);

		HASH_ADD_KEYPTR(hh, engine_inmemory->instance, tmp->key, strlen(tmp->key), tmp);
	} else {
		return -1;
	}

	return 0;
}

static char *
get(engine_base_t *engine,
	const char *key,
	size_t key_len,
	size_t *value_len)
{
	char *value;
	engine_inmemory_instance_t *tmp = NULL;
	engine_inmemory_t *engine_inmemory = (engine_inmemory_t *)engine;

	HASH_FIND_STR(engine_inmemory->instance, key, tmp);
	if (tmp == NULL) {
		*value_len = 0;

		return NULL;
	} else {
		int tmpval_len = strlen(tmp->value);
		value = (char *)malloc(sizeof(char) * (tmpval_len + 1));
		memset(value, 0, (tmpval_len + 1));
		strncpy(value, tmp->value, tmpval_len);
		*value_len = tmpval_len;
	}

	return value;
}

static int
delete(engine_base_t *engine,
	   const char *key,
	   size_t key_len)
{
	engine_inmemory_instance_t *tmp = NULL;
	engine_inmemory_t *engine_inmemory = (engine_inmemory_t *)engine;

	HASH_FIND_STR(engine_inmemory->instance, key, tmp);
	if (tmp == NULL) {
		
		return 0;
	} else {
		HASH_DEL(engine_inmemory->instance, tmp);
		if (tmp->key != NULL) free(tmp->key);
		if (tmp->value != NULL) free(tmp->value);
	}

	return 0;
}

engine_inmemory_t *
engine_inmemory_init(void)
{
	engine_inmemory_t *engine = (engine_inmemory_t *)malloc(sizeof(engine_inmemory_t));

	engine->config = (engine_inmemory_config_t *) malloc(sizeof(engine_inmemory_config_t));
	engine->instance = NULL;

	engine_operation_t *engine_ops = (engine_operation_t *)malloc(sizeof(engine_operation_t));

	const char *engine_name = "inmemory engine v0.1";
	size_t engine_name_len = strlen(engine_name);
	engine->base.name = malloc(sizeof(char) * (engine_name_len + 1));
	memset(engine->base.name, 0, (engine_name_len + 1));
	strncpy(engine->base.name, engine_name, engine_name_len);

	size_t version = 0x1;
	engine->base.version = version;

	engine_ops->put    = put;
	engine_ops->get    = get;
	engine_ops->delete = delete;

	engine->base.engine_ops = engine_ops;

	return engine;
}
