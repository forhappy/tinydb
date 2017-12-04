/*
 * =============================================================================
 *
 *       Filename:  csas.c
 *
 *    Description:  common storage abstract service.
 *
 *        Created:  11/24/2012 03:16:38 PM
 *
 *         Author:  Fu Haiping (forhappy), haipingf@gmail.com
 *        Company:  ICT ( Institute Of Computing Technology, CAS )
 *
 * =============================================================================
 */

#include "csas.h"

csas_context_t *
csas_init(engine_base_t *engine)
{
	csas_context_t *context = (csas_context_t *)malloc(sizeof(csas_context_t));

	context->engine = engine;
	context->next = NULL;

	return context;
}

int
csas_put(csas_context_t *context,
		 const char *key,
		 size_t key_len,
		 const char *value,
		 size_t value_len)
{
	int ret = -1;

	ret = context->engine->engine_ops->put(context->engine, key, key_len, value, value_len);

	return ret;
}

char *
csas_get(csas_context_t *context,
		const char *key,
		size_t key_len,
		size_t *value_len)
{
	char *retstr = NULL;

	retstr = context->engine->engine_ops->get(context->engine, key, key_len, value_len);

	return retstr;
}

int
csas_delete(csas_context_t *context,
		    const char *key,
			size_t key_len)
{
	int ret = -1;

	ret = context->engine->engine_ops->del(context->engine, key, key_len);

	return ret;
}

void
csas_destory(csas_context_t *context)
{
	assert(context != NULL);

	if (context->engine != NULL) {
		context->engine->engine_ops->quit(context->engine);
		free(context->engine);
	}
	free(context);
}
