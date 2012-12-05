/*
 * =============================================================================
 *
 *       Filename:  csas.h
 *
 *    Description:  common storage abstract service.
 *
 *        Created:  11/24/2012 03:16:19 PM
 *
 *         Author:  Fu Haiping (forhappy), haipingf@gmail.com
 *        Company:  ICT ( Institute Of Computing Technology, CAS )
 *
 * =============================================================================
 */
#ifndef CSAS_H
#define CSAS_H

typedef struct engine_base_s_ engine_base_t;
typedef struct engine_operation_s_ engine_operation_t;
typedef struct csas_context_s_ csas_context_t;

struct csas_context_s_ {
	csas_context_t *next;
	engine_base_t *engine;
	/* other stuff here... */
};

struct engine_base_s_ {
	char *name;
	unsigned int version;
	engine_operation_t *engine_ops;
	/* more stuff about storage engine base here... */
};

struct engine_operation_s_ {
	int (*put)(engine_base_t *engine,
			const char *key, unsigned int key_len,
			const char *value, unsigned int value_len);

	char * (*get)(engine_base_t *engine,
			const char *key, unsigned int key_len,
			unsigned int *value_len);

	int (*delete)(engine_base_t *engine,
			const char *key, unsigned int key_len);

	int (*unsupported_operation)(engine_base_t *engine);
};

csas_context_t * csas_init(engine_base_t *engine);

void csas_destory(csas_context_t *context);

int csas_put(csas_context_t *context,
		const char *key, unsigned int key_len,
		const char *value, unsigned int value_len);

char * csas_get(csas_context_t *context,
		const char *key, unsigned int key_len,
		unsigned int *value_len);

int csas_delete(csas_context_t *context,
		const char *key, unsigned int key_len);

#endif /* CSAS_H */
