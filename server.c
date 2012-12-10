/*
 * =============================================================================
 *
 *       Filename:  net.c
 *
 *    Description:  network connection utility.
 *
 *        Created:  12/06/2012 08:24:19 PM
 *
 *         Author:  Fu Haiping (forhappy), haipingf@gmail.com
 *        Company:  ICT ( Institute Of Computing Technology, CAS )
 *
 * =============================================================================
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#include "evhtp/evhtp.h"

#define TINYDB_USE_ENGINE_LEVELDB

#include "server.h"

#if defined(TINYDB_USE_ENGINE_LEVELDB)
#include "leveldb-engine.h"
#else
#include "inmemory-engine.h"
#endif


#define TINYDB_MAX_KV_RESPONSE_BUFFER_SIZE (1024 * 1024 *2)
#define TINYDB_MAX_ERROR_RESPONSE_BUFFER_SIZE 1024

static void URI_newdb_cb(evhtp_request_t *req, void *userdata);
static void URI_get_cb(evhtp_request_t *req, void *userdata);
static void URI_set_cb(evhtp_request_t *req, void *userdata);
static void URI_delete_cb(evhtp_request_t *req, void *userdata);

database_t *
tinydb_database_init(const char *dbname, csas_context_t *context)
{
	database_t *db = (database_t *)malloc(sizeof(database_t));
	if (db == NULL) {
		fprintf(stderr, "tinydb_database_init() failed due to out of memory.\n");
		return NULL;
	}
	unsigned int dbname_len = strlen(dbname);
	db->dbname = (char *)malloc(sizeof(char) * (dbname_len + 1));
	memset(db->dbname, 0, (dbname_len + 1));
	strncpy(db->dbname, dbname, dbname_len);

	db->context = context;
	db->next = NULL;

	return db;
}

server_t * tinydb_server_init(database_t *default_db)
{
	server_t *server = (server_t *)malloc(sizeof(server_t));
	if (server == NULL) {
		fprintf(stderr, "tinydb_server_init() failed due to out of memory.\n");
		return NULL;
	}

    evbase_t * evbase = event_base_new();
    evhtp_t * htp    = evhtp_new(evbase, NULL);
	server->evbase = evbase;
	server->htp = htp;
	server->db = default_db;

	return server;
}

void
tinydb_server_add_database(server_t *server, database_t *database)
{
	assert(server != NULL);
	assert(database != NULL);

	database_t *db = (database_t *)server->db;

	if (db != NULL) {
		database->next = db;
		server->db = database;
	} else server->db = database;
}

void tinydb_main_loop(server_t *server)
{
	server->newdb_cb = evhtp_set_cb(server->htp, "/new", URI_newdb_cb, server);

    evhtp_bind_socket(server->htp, "0.0.0.0", 8088, 1024);
    event_base_loop(server->evbase, 0);

    evhtp_unbind_socket(server->htp);
    evhtp_callback_free(server->newdb_cb);
    evhtp_free(server->htp);
    event_base_free(server->evbase);

}

static char *
tinydb_jsonfy_kv_response(const char *key, const char *val)
{
	assert(key != NULL);
	assert(val != NULL);

	char *response = (char *)malloc(sizeof(char) *
			TINYDB_MAX_KV_RESPONSE_BUFFER_SIZE);
	if (response == NULL) {
		fprintf(stderr, "malloc error due to out of memory.\n");
		return NULL;
	} else {
		sprintf(response, "{\"key\": \"%s\",\"val\":\"%s\"}", key, val);
	}
	return response;
}

static char *
tinydb_jsonfy_error_response(const char *err, const char *msg)
{
	assert(err != NULL);
	assert(msg != NULL);

	char *response = (char *)malloc(sizeof(char) *
			TINYDB_MAX_ERROR_RESPONSE_BUFFER_SIZE);
	if (response == NULL) {
		fprintf(stderr, "malloc error due to out of memory.\n");
		return NULL;
	} else {
		sprintf(response, "{\"err\": \"%s\",\"msg\":\"%s\"}", err, msg);
	}
	return response;
}

static void
URI_newdb_cb(evhtp_request_t *req, void *userdata)
{
	assert(userdata != NULL);
	server_t *server = (server_t *)userdata;
	/* json formatted response. */
	char *response = NULL;
	char *value = NULL;
	unsigned int value_len = -1;
	char get_cb_path[64] = {0};
	char set_cb_path[64] = {0};
	char del_cb_path[64] = {0};

	/* HTTP protocol used */
	evhtp_proto proto = req->proto;
	if (proto != EVHTP_PROTO_11) {
		response = tinydb_jsonfy_error_response("ProtocalError",
				"Protocal error, you may have to use HTTP/1.1 to do request.");
		evbuffer_add_printf(req->buffer_out, "%s", response);
		evhtp_send_reply(req, EVHTP_RES_OK);
		free(response);
		return;
	}
	/* request method. */
	int method= evhtp_request_get_method(req);
	if (method != htp_method_GET) {
		response = tinydb_jsonfy_error_response("HTTPMethodError",
				"HTTP method error, you may have to use GET to do request.");
		evbuffer_add_printf(req->buffer_out, "%s", response);
		free(response);
		evhtp_send_reply(req, EVHTP_RES_OK);
		return;
	}

	/* request query from client */
	evhtp_query_t *query = req->uri->query;
	const char *dbname = evhtp_kv_find(query, "dbname");

	engine_leveldb_config_t *config = engine_leveldb_config_init(dbname);
	engine_base_t *engine_leveldb = (engine_base_t *)engine_leveldb_init(config);
	csas_context_t *context = csas_init(engine_leveldb);
	database_t *database = tinydb_database_init(dbname, context);

	tinydb_server_add_database(server, database);


	if (dbname == NULL) return;
	sprintf(get_cb_path, "/%s/get", dbname);
	sprintf(set_cb_path, "/%s/set", dbname);
	sprintf(del_cb_path, "/%s/del", dbname);

    evhtp_set_cb(server->htp, get_cb_path, URI_get_cb, server);
    evhtp_set_cb(server->htp, set_cb_path, URI_set_cb, server);
    evhtp_set_cb(server->htp, del_cb_path, URI_delete_cb, server);

	return;
}

static void
URI_get_cb(evhtp_request_t *req, void *userdata)
{
	assert(userdata != NULL);
	
	server_t *server = (server_t *)userdata;
	database_t *database = (database_t *)server->db;

	csas_context_t *context = NULL;
	/* json formatted response. */
	char *response = NULL;
	char *value = NULL;
	unsigned int value_len = -1;
	char dbname[64] = {0};

	/* HTTP protocol used */
	evhtp_proto proto = req->proto;
	if (proto != EVHTP_PROTO_11) {
		response = tinydb_jsonfy_error_response("ProtocalError",
				"Protocal error, you may have to use HTTP/1.1 to do request.");
		evbuffer_add_printf(req->buffer_out, "%s", response);
		evhtp_send_reply(req, EVHTP_RES_OK);
		free(response);
		return;
	}
	/* request method. */
	int method= evhtp_request_get_method(req);
	if (method != htp_method_GET) {
		response = tinydb_jsonfy_error_response("HTTPMethodError",
				"HTTP method error, you may have to use GET to do request.");
		evbuffer_add_printf(req->buffer_out, "%s", response);
		free(response);
		evhtp_send_reply(req, EVHTP_RES_OK);
		return;
	}
	/* request database name. */
	const char *path = req->uri->path->path;
	char *second_slash = strchr(path + 1, '/');
	strncpy(dbname, path + 1, second_slash - path -1);
	while(database->next != NULL) {
		if (strcmp(dbname, database->dbname) == 0) {
			context = database->context;
			break;
		}
		database = database->next;
	}

	/* request query from client */
	evhtp_query_t *query = req->uri->query;
	const char *key = evhtp_kv_find(query, "key");
	value = csas_get(context, key, strlen(key), &value_len);
	if (value != NULL) {
		char *buf = (char *)malloc(sizeof(char) * (value_len + 1));
		memset(buf, 0, value_len + 1);
		snprintf(buf, value_len + 1, "%s", value);
		response = tinydb_jsonfy_kv_response(key, buf);
		evbuffer_add_printf(req->buffer_out, "%s", response);
		evhtp_send_reply(req, EVHTP_RES_OK);
		
		free(buf);
		free(value);
		free(response);
	} else {
		response = tinydb_jsonfy_error_response("NoSuchKey", "No such key exists, please check agein.");
		evbuffer_add_printf(req->buffer_out, "%s", response);
		free(response);
		evhtp_send_reply(req, EVHTP_RES_OK);
	}

	return;
}

static void
URI_set_cb(evhtp_request_t *req, void *userdata)
{
	assert(userdata != NULL);

	server_t *server = (server_t *)userdata;
	database_t *database = (database_t *)server->db;

	csas_context_t *context = (csas_context_t *)userdata;
	/* json formatted response. */
	char *response = NULL;
	char dbname[64] = {0};
	int ret = -1;

	/* HTTP protocol used */
	evhtp_proto proto = req->proto;
	if (proto != EVHTP_PROTO_11) {
		response = tinydb_jsonfy_error_response("ProtocalError",
				"Protocal error, you may have to use HTTP/1.1 to do request.");
		evbuffer_add_printf(req->buffer_out, "%s", response);
		evhtp_send_reply(req, EVHTP_RES_OK);
		free(response);
		return;
	}
	/* request method. */
	int method= evhtp_request_get_method(req);
	if (method != htp_method_GET) {
		response = tinydb_jsonfy_error_response("HTTPMethodError",
				"HTTP method error, you may have to use GET to do request.");
		evbuffer_add_printf(req->buffer_out, "%s", response);
		free(response);
		evhtp_send_reply(req, EVHTP_RES_OK);
		return;
	}
	/* request database name. */
	const char *path = req->uri->path->path;
	char *second_slash = strchr(path + 1, '/');
	strncpy(dbname, path + 1, second_slash - path -1);
	while(database->next != NULL) {
		if (strcmp(dbname, database->dbname) == 0) {
			context = database->context;
			break;
		}
		database = database->next;
	}
	/* request query from client */
	evhtp_query_t *query = req->uri->query;
	const char *key = evhtp_kv_find(query, "key");
	const char *value = evhtp_kv_find(query, "value");
	ret = csas_put(context, key, strlen(key), value, strlen(value));
	if (ret < 0) {
		response = tinydb_jsonfy_error_response("SetKVError", "Set Key-Value pair error.");
	} else {
		response = tinydb_jsonfy_error_response("OK", "Set key successfully.");
	}

    evbuffer_add_printf(req->buffer_out, "%s", response);
    evhtp_send_reply(req, EVHTP_RES_OK);

	free(response);
	return;
}

static void
URI_delete_cb(evhtp_request_t *req, void *userdata)
{
	assert(userdata != NULL);

	server_t *server = (server_t *)userdata;
	database_t *database = (database_t *)server->db;

	csas_context_t *context = (csas_context_t *)userdata;
	/* json formatted response. */
	char *response = NULL;
	char *value = NULL;
	char dbname[64] = {0};
	int ret = -1;

	/* HTTP protocol used */
	evhtp_proto proto = req->proto;
	if (proto != EVHTP_PROTO_11) {
		response = tinydb_jsonfy_error_response("ProtocalError",
				"Protocal error, you may have to use HTTP/1.1 to do request.");
		evbuffer_add_printf(req->buffer_out, "%s", response);
		evhtp_send_reply(req, EVHTP_RES_OK);
		free(response);
		return;
	}
	/* request method. */
	int method= evhtp_request_get_method(req);
	if (method != htp_method_GET) {
		response = tinydb_jsonfy_error_response("HTTPMethodError",
				"HTTP method error, you may have to use GET to do request.");
		evbuffer_add_printf(req->buffer_out, "%s", response);
		free(response);
		evhtp_send_reply(req, EVHTP_RES_OK);
		return;
	}
	/* request database name. */
	const char *path = req->uri->path->path;
	char *second_slash = strchr(path + 1, '/');
	strncpy(dbname, path + 1, second_slash - path -1);
	while(database->next != NULL) {
		if (strcmp(dbname, database->dbname) == 0) {
			context = database->context;
			break;
		}
		database = database->next;
	}
	/* request query from client */
	evhtp_query_t *query = req->uri->query;
	const char *key = evhtp_kv_find(query, "key");
	ret = csas_delete(context, key, strlen(key));
	if (ret < 0) {
		response = tinydb_jsonfy_error_response("DeleteKVError", "Delete Key-Value pair error.");
	} else {
		response = tinydb_jsonfy_error_response("OK", "Delete key successfully.");
	}
    evbuffer_add_printf(req->buffer_out, "%s", response);
    evhtp_send_reply(req, EVHTP_RES_OK);

	free(response);
	return;
}

static const char * dbname = "/tmp/leveldb";

int 
main(int argc, char ** argv)
{

#if defined(TINYDB_USE_ENGINE_LEVELDB)
	engine_leveldb_config_t *config = engine_leveldb_config_init(dbname);
	engine_base_t *engine_leveldb = (engine_base_t *)engine_leveldb_init(config);
	csas_context_t *context = csas_init(engine_leveldb);
#else
	engine_base_t *engine_inmemory = (engine_base_t *)engine_inmemory_init();
	csas_context_t *context = csas_init(engine_inmemory);
#endif

	database_t *database = tinydb_database_init(dbname, context);

	server_t *server = tinydb_server_init(database);

	tinydb_main_loop(server);

    return 0;
}

