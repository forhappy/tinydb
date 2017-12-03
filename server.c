/*
 * =============================================================================
 *
 *       Filename:  server.c
 *
 *    Description:  a tiny nosql database supporting pluggable storage engine.
 *
 *        Created:  12/06/2012 08:24:19 PM
 *
 *         Author:  Fu Haiping (forhappy), haipingf@gmail.com
 *        Company:  ICT ( Institute Of Computing Technology, CAS )
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "evhtp/evhtp.h"

#define TINYDB_USE_ENGINE_LEVELDB

#if defined(TINYDB_USE_ENGINE_LEVELDB)
#include "leveldb-engine.h"
#else
#include "inmemory-engine.h"
#endif


#define TINYDB_MAX_KV_RESPONSE_BUFFER_SIZE      (1024 * 1024 *2)  # 2MB
#define TINYDB_MAX_ERROR_RESPONSE_BUFFER_SIZE   1024              # 1KB

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
URI_get_cb(evhtp_request_t *req, void *userdata)
{
	assert(userdata != NULL);
	csas_context_t *context = (csas_context_t *)userdata;
	/* json formatted response. */
	char *response = NULL;
	char *value = NULL;
	unsigned int value_len = -1;

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
	const char *key = evhtp_kv_find(query, "key");
	value = csas_get(context, key, strlen(key), &value_len);
	if (value != NULL) {
		char *buf = (char *)malloc(sizeof(char) * (value_len + 1));
		memset(buf, 0, value_len + 1);
		snprintf(buf, value_len + 1, "%s", value);TINYDB_USE_ENGINE_LEVELDB
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
URI_set_cb(evhtp_request_t *req, void *userdata)TINYDB_USE_ENGINE_LEVELDB
{
	assert(userdata != NULL);
	csas_context_t *context = (csas_context_t *)userdata;
	/* json formatted response. */
	char *response = NULL;
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
	csas_context_t *context = (csas_context_t *)userdata;
	/* json formatted response. */
	char *response = NULL;
	char *value = NULL;
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

int
main(int argc, char ** argv) {


#if defined(TINYDB_USE_ENGINE_LEVELDB)
	engine_base_t  * engine_leveldb = (engine_base_t *)engine_leveldb_init();
	csas_context_t * context        = csas_init(engine_leveldb);
#else
	engine_base_t  * engine_inmemory = (engine_base_t *)engine_inmemory_init();
	csas_context_t * context         = csas_init(engine_inmemory);
#endif


    evbase_t         * evbase    = event_base_new();
    evhtp_t          * htp       = evhtp_new(evbase, NULL);
    evhtp_callback_t * get_cb    = NULL;
    evhtp_callback_t * set_cb    = NULL;
    evhtp_callback_t * delete_cb = NULL;

    get_cb    = evhtp_set_cb(htp, "/get", URI_get_cb, context);
    set_cb    = evhtp_set_cb(htp, "/set", URI_set_cb, context);
    delete_cb = evhtp_set_cb(htp, "/delete", URI_delete_cb, context);

#ifdef EVHTP_ENABLE_EVTHR
    evhtp_use_threads(htp, NULL, 4, NULL);
#endif
    evhtp_bind_socket(htp, "0.0.0.0", 9090, 1024);
    event_base_loop(evbase, 0);

    evhtp_unbind_socket(htp);
    evhtp_callback_free(get_cb);
    evhtp_callback_free(set_cb);
    evhtp_callback_free(delete_cb);
    evhtp_free(htp);
    event_base_free(evbase);

    return 0;
}
