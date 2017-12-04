/*
 * =============================================================================
 *
 *       Filename:  leveldb-engine.h
 *
 *    Description:  leveldb storage engine
 *
 *        Created:  11/24/2012 03:49:23 PM
 *
 *         Author:  Fu Haiping (forhappy), haipingf@gmail.com
 *        Company:  ICT ( Institute Of Computing Technology, CAS )
 *
 * =============================================================================
 */

#ifndef __LEVELDB_ENGINE_H__
#define __LEVELDB_ENGINE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <leveldb/c.h>

#include "../csas.h"

typedef struct engine_leveldb_config_s_ engine_leveldb_config_t;
typedef struct engine_leveldb_instance_s_ engine_leveldb_instance_t;
typedef struct engine_leveldb_s_ engine_leveldb_t;

static char *dbname = "/tmp/leveldb";

/* 
 * leveldb config
 */
struct engine_leveldb_config_s_ {
	char     *dbname;                 /** default database name */
	bool     create_if_missing;       /** create database if it doesn't exist */
	bool     error_if_exist;          /** open database throws an error if it do exist */
	size_t   lru_cache_size;          /** leveldb's lru cache size */
	size_t   write_buffer_size;       /** leveldb's write buffer size */
	bool     paranoid_checks;         /** paranoid checks */
	size_t   max_open_files;          /** max open files */
	unsigned block_size;              /** block size */
	size_t   block_restart_interval;  /** block restart interval */
	bool     compression_support;     /** compression support, 0: no compression, 1: snappy compression */
	bool     verify_checksums;        /** set true to verify checksums when read */
	bool     fill_cache;              /** set true if want to fill cache */
	bool     sync;                    /** set true to enable sync when write */
};

/* 
 * leveldb instance
 */
struct engine_leveldb_instance_s_ {
	leveldb_t              *db;
	leveldb_comparator_t   *comparator;
	leveldb_env_t          *env;
	leveldb_cache_t        *cache;
	leveldb_filterpolicy_t *filterpolicy;
	leveldb_iterator_t     *iterator;
	leveldb_logger_t       *logger;
	leveldb_readoptions_t  *roptions;
	leveldb_snapshot_t     *snapshot;
	leveldb_writebatch_t   *writebatch;
	leveldb_options_t      *options;
	leveldb_writeoptions_t *woptions;
	char                   *err;
};

/* 
 * leveldb
 */
struct engine_leveldb_s_ {
	/*
	 * engine base goes here, it MUST be a struct object, and NOT a struct pointer. 
	 */
	engine_base_t             base;
	engine_leveldb_config_t   *config;
	engine_leveldb_instance_t *instance;
};

extern engine_leveldb_t * engine_leveldb_init();

#endif /* __LEVELDB_ENGINE_H__ */
