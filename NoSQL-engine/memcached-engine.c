/*
 * =============================================================================
 *
 *       Filename:  memcached-engine.c
 *
 *    Description:  memcached storage engine.
 *
 *        Created:  11/24/2012 03:49:34 PM
 *
 *         Author:  Fu Haiping (forhappy), haipingf@gmail.com
 *        Company:  ICT ( Institute Of Computing Technology, CAS )
 *
 * =============================================================================
 */

#include "memcached-engine.h"

static int
put(engine_base_t *engine,
	const char *key,
	size_t key_len,
	const char *value,
	size_t value_len)
{
	engine_memcached_t *engine_memcached = (engine_memcached_t *)engine;
	memcached_st *context = engine_memcached->instance->context;
	memcached_return_t reply;

	/*
	 * memcached_set key存在则替换，不存在则存入
	 * memcached_add key存在则报错，不存在则存入
	 * memcached_replace key存在则替换，不存在则报错
	 */
	reply = memcached_set(context, key, key_len, value, value_len, (time_t)60, 0);
	if (reply != MEMCACHED_SUCCESS) {
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
	char* value;
	engine_memcached_t *engine_memcached = (engine_memcached_t *)engine;
	memcached_st *context = engine_memcached->instance->context;
	memcached_return_t reply;

	value = memcached_get(context, key, key_len, value_len, 0, &reply);
	if (reply != MEMCACHED_SUCCESS) {
		*value_len = 0;
		value = NULL;
	}

	return value;
}

static int
del(engine_base_t *engine,
	const char *key,
	size_t key_len)
{
	engine_memcached_t *engine_memcached = (engine_memcached_t *)engine;
	memcached_st *context = engine_memcached->instance->context;
	memcached_return_t reply;

	reply = memcached_delete(context, key, key_len, (time_t)0);  // TODO
	if (reply != MEMCACHED_SUCCESS) {
		return -1;
	}

	return 0;
}

static void
quit(engine_base_t *engine)
{
	engine_memcached_t *engine_memcached = (engine_memcached_t *)engine;
	memcached_st *context = engine_memcached->instance->context;

	memcached_quit(context);
	memcached_free(context);
	
	free(engine_memcached->config);
	free(engine_memcached->instance);
} 

static engine_memcached_config_t *
engine_memcached_config_init()
{
	engine_memcached_config_t *config = (engine_memcached_config_t *)malloc(sizeof(engine_memcached_config_t));

	config->ip   = "127.0.0.1";
	config->port = 11211;

	return config;
}

static engine_memcached_instance_t *
engine_memcached_instance_init(engine_memcached_config_t *config)
{
	engine_memcached_instance_t *instance = (engine_memcached_instance_t *)malloc(sizeof(engine_memcached_instance_t));

	instance->context = memcached_create(NULL);
	memcached_server_add(instance->context, config->ip, config->port);

	return instance;
}

engine_memcached_t *
engine_memcached_init()
{
	engine_memcached_t *engine = (engine_memcached_t *) malloc(sizeof(engine_memcached_t));

	engine_memcached_config_t   *config   = engine_memcached_config_init();
	engine_memcached_instance_t *instance = engine_memcached_instance_init(config);

	engine->config   = config;
	engine->instance = instance;

	engine_operation_t *engine_ops = (engine_operation_t *)malloc(sizeof(engine_operation_t));

	const char *engine_name = "memcached engine v0.1";
	size_t engine_name_len = strlen(engine_name);
	engine->base.name = malloc(sizeof(char) * (engine_name_len + 1));
	memset(engine->base.name, 0, (engine_name_len + 1));
	strncpy(engine->base.name, engine_name, engine_name_len);

	size_t version = 0x1;
	engine->base.version = version;

	engine_ops->put  = put;
	engine_ops->get  = get;
	engine_ops->del  = del;
	engine_ops->quit = quit;

	engine->base.engine_ops = engine_ops;

	return engine;
}
