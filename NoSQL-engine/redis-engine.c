/*
 * =============================================================================
 *
 *       Filename:  redis-engine.c
 *
 *    Description:  redis storage engine.
 *
 *        Created:  11/24/2012 03:49:34 PM
 *
 *         Author:  Fu Haiping (forhappy), haipingf@gmail.com
 *        Company:  ICT ( Institute Of Computing Technology, CAS )
 *
 * =============================================================================
 */

#include "redis-engine.h"

static int
put(engine_base_t *engine,
	const char *key,
	size_t key_len,
	const char *value,
	size_t value_len)
{
	engine_redis_t *engine_redis = (engine_redis_t *)engine;
	redisContext *context = engine_redis->instance->context;
	redisReply *reply;

	reply = redisCommand(context, "SET %b %b", key, key_len, value, value_len);
	if (reply->type == REDIS_REPLY_ERROR) {
		freeReplyObject(reply);

		return -1;
	} else {
		freeReplyObject(reply);
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
	engine_redis_t *engine_redis = (engine_redis_t *)engine;
	redisContext *context = engine_redis->instance->context;
	redisReply *reply;

	reply = redisCommand(context, "GET %s", key);
	if (reply->type == REDIS_REPLY_ERROR) {
		*value_len = 0;
		freeReplyObject(reply);
		value = NULL;
	} else if (reply->type == REDIS_REPLY_STRING) {
		int tmpval_len = reply->len;
		value = (char *)malloc(sizeof(char) * (tmpval_len + 1));
		memset(value, 0, (tmpval_len + 1));
		strncpy(value, reply->str, tmpval_len);
		*value_len = tmpval_len;

		freeReplyObject(reply);
	} else {
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
	engine_redis_t *engine_redis = (engine_redis_t *)engine;
	redisContext *context = engine_redis->instance->context;
	redisReply *reply;

	reply = redisCommand(context, "DEL %s", key);
	if (reply->type == REDIS_REPLY_ERROR) {
		freeReplyObject(reply);

		return -1;
	} else {
		freeReplyObject(reply);
	}

	return 0;
}

static void
quit(engine_base_t *engine)
{
	engine_redis_t *engine_redis = (engine_redis_t *)engine;
	redisContext *context = engine_redis->instance->context;

	redisFree(context);
	
	free(engine_redis->config);
	free(engine_redis->instance);
} 

static engine_redis_config_t *
engine_redis_config_init()
{
	engine_redis_config_t *config = (engine_redis_config_t *)malloc(sizeof(engine_redis_config_t));

	config->ip   = "127.0.0.1";
	config->port = 6379;

	return config;
}

static engine_redis_instance_t *
engine_redis_instance_init(engine_redis_config_t *config)
{
	engine_redis_instance_t *instance = (engine_redis_instance_t *)malloc(sizeof(engine_redis_instance_t));

	redisContext *c = redisConnect(config->ip, config->port);
	if (c == NULL || c->err) {
		if (c) {
			printf("Error: %s\n", c->errstr);
		} else {
			fprintf(stderr, "Can't allocate redis context\n");
		}
		
		return NULL;
	}
	instance->context = c;

	return instance;
}

engine_redis_t *
engine_redis_init()
{
	engine_redis_t *engine = (engine_redis_t *) malloc(sizeof(engine_redis_t));

	engine_redis_config_t   *config   = engine_redis_config_init();
	engine_redis_instance_t *instance = engine_redis_instance_init(config);

	engine->config = config;
	engine->instance = instance;

	engine_operation_t *engine_ops = (engine_operation_t *)malloc(sizeof(engine_operation_t));

	const char *engine_name = "redis engine v0.1";
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
