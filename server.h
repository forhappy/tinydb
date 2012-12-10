/*
 * =============================================================================
 *
 *       Filename:  server.h
 *
 *    Description:  server instance.
 *
 *        Created:  12/09/2012 09:31:11 PM
 *
 *         Author:  Fu Haiping (forhappy), haipingf@gmail.com
 *        Company:  ICT ( Institute Of Computing Technology, CAS )
 *
 * =============================================================================
 */
#ifndef SERVER_H
#define SERVER_H
#include "csas.h"

typedef struct server_s_ server_t;
typedef struct database_s_ database_t;

struct server_s_ {
	database_t *db;
	
	evbase_t         * evbase;
    evhtp_t          * htp;
	
    evhtp_callback_t * newdb_cb;
};

struct database_s_ {
	char *dbname;
	csas_context_t *context;
	database_t *next;
};

extern database_t * tinydb_database_init(const char *dbname,
		csas_context_t *context);

extern server_t * tinydb_server_init(database_t *default_db);

#endif // SERVER_H
