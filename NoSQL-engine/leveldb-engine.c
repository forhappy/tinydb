/*
 * =============================================================================
 *
 *       Filename:  leveldb-engine.c
 *
 *    Description:  leveldb storage engine.
 *
 *        Created:  11/24/2012 03:49:34 PM
 *
 *         Author:  Fu Haiping (forhappy), haipingf@gmail.com
 *        Company:  ICT ( Institute Of Computing Technology, CAS )
 *
 * =============================================================================
 */

#include "leveldb-engine.h"

static int
put(engine_base_t *engine,
    const char *key,
	size_t key_len,
	const char *value,
	size_t value_len)
{
	engine_leveldb_t *engine_leveldb = (engine_leveldb_t *)engine;
	engine_leveldb_instance_t *instance = engine_leveldb->instance;

	leveldb_put(instance->db, instance->woptions, key, key_len, value, value_len, &(instance->err));

	return 0;
}

static char *
get(engine_base_t *engine,
	const char *key,
	size_t key_len,
	size_t *value_len)
{
	char* value;
	engine_leveldb_t *engine_leveldb = (engine_leveldb_t *)engine;
	engine_leveldb_instance_t *instance = engine_leveldb->instance;

	value = leveldb_get(instance->db, instance->roptions, key, key_len, value_len, &(instance->err));

	return value;
}

static int
delete(engine_base_t *engine,
	   const char *key,
	   size_t key_len)
{
	engine_leveldb_t *engine_leveldb = (engine_leveldb_t *)engine;
	engine_leveldb_instance_t *instance = engine_leveldb->instance;

    leveldb_delete(instance->db, instance->woptions, key, key_len, &(instance->err));
	
	return 0;
}

static engine_leveldb_config_t *
engine_leveldb_config_init(void)
{
	engine_leveldb_config_t *config = (engine_leveldb_config_t *)malloc(sizeof(engine_leveldb_config_t));

	size_t dbname_len = strlen(dbname);
	config->dbname = (char *)malloc(sizeof(char) * (dbname_len + 1));
	memset(config->dbname, 0, (dbname_len + 1));
	strncpy(config->dbname, dbname, dbname_len);

	config->create_if_missing      = true;
	config->error_if_exist         = false;
	config->lru_cache_size         = 65536;
	config->write_buffer_size      = 65536;
	config->paranoid_checks        = true;
	config->max_open_files         = 32;
	config->block_size             = 1024;
	config->block_restart_interval = 8;
	config->compression_support    = false;
	config->verify_checksums       = false;
	config->fill_cache             = false;
	config->sync                   = false;

	return config;
}

static engine_leveldb_instance_t *
engine_leveldb_instance_init(engine_leveldb_config_t *config)
{
	engine_leveldb_instance_t *instance = (engine_leveldb_instance_t *)malloc(sizeof(engine_leveldb_instance_t));
	
	instance->comparator   = NULL;
	instance->env          = leveldb_create_default_env();
	instance->cache        = leveldb_cache_create_lru(config->lru_cache_size);
	instance->filterpolicy = NULL;
	instance->iterator     = NULL;
	instance->logger       = NULL;
	instance->snapshot     = NULL;
	instance->writebatch   = NULL;

	instance->options = leveldb_options_create();
	leveldb_options_set_create_if_missing(instance->options, config->create_if_missing);
	leveldb_options_set_error_if_exists(instance->options, config->error_if_exist);
	leveldb_options_set_cache(instance->options, instance->cache);
	leveldb_options_set_env(instance->options, instance->env);
	leveldb_options_set_info_log(instance->options, NULL);
	leveldb_options_set_write_buffer_size(instance->options, config->write_buffer_size);
	leveldb_options_set_paranoid_checks(instance->options, config->paranoid_checks);
	leveldb_options_set_max_open_files(instance->options, 10);
	leveldb_options_set_block_size(instance->options, 1024);
	leveldb_options_set_block_restart_interval(instance->options, 8);
	leveldb_options_set_compression(instance->options, leveldb_no_compression);
	
	instance->roptions = leveldb_readoptions_create();
	leveldb_readoptions_set_verify_checksums(instance->roptions, config->verify_checksums);
	leveldb_readoptions_set_fill_cache(instance->roptions, config->fill_cache);
	
	instance->woptions = leveldb_writeoptions_create();
	leveldb_writeoptions_set_sync(instance->woptions, config->sync);
	
	instance->db = leveldb_open(instance->options, config->dbname, &(instance->err));

	return instance;
}

engine_leveldb_t *
engine_leveldb_init()
{
	engine_leveldb_t *engine = (engine_leveldb_t *)malloc(sizeof(engine_leveldb_t));

	engine_leveldb_config_t   *config   = engine_leveldb_config_init();
	engine_leveldb_instance_t *instance = engine_leveldb_instance_init(config);

	engine->config = config;
	engine->instance = instance;

	engine_operation_t *engine_ops = (engine_operation_t *)malloc(sizeof(engine_operation_t));

	const char *engine_name = "leveldb engine v0.1";
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
