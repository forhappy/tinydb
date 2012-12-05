/*
 * =============================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  main routine.
 *
 *        Created:  11/24/2012 05:58:22 PM
 *
 *         Author:  Fu Haiping (forhappy), haipingf@gmail.com
 *        Company:  ICT ( Institute Of Computing Technology, CAS )
 *
 * =============================================================================
 */
#include <stdio.h>
#include <string.h>

#include "leveldb-engine.h"
#include "inmemory-engine.h"
#define NUM_OPERATIONS 65536

int main(int args, const char *argv[])
{
	char buf[1024] = {0};
	char *value = NULL;
	unsigned int value_len = -1;

#if 0 // leveldb engine.
	engine_base_t *engine_leveldb = (engine_base_t *)engine_leveldb_init();
	csas_context_t *context = csas_init(engine_leveldb);
#endif

#if 1 // in-memory engine.
	engine_base_t *engine_inmemory = (engine_base_t *)engine_inmemory_init();
	csas_context_t *context = csas_init(engine_inmemory);
#endif


#if 1
	/* put operation. */

	char key[64] = {0};
	char val[64] = {0};
	for (int i = 0; i < NUM_OPERATIONS; i++) {
		sprintf(key, "%d%d%d%d%d", i, i + 1, i + 2, i + 3, i + 4);
		sprintf(val, "%d%d%d%d%d", i, i + 1, i + 2, i + 3, i + 4 );
		csas_put(context, key, strlen(key), val, strlen(val));
	}
#endif

#if 1
	/* get operation. */
	value = csas_get(context, "910111213", 5, &value_len);
	snprintf(buf, value_len + 1, "%s", value);
	printf("value: %s\tlength: %d\n", buf, strlen(buf));
	free(value);

#endif

#if 1
	/* delete operation. */
	csas_delete(context, "12345", 5);
	/* verify deletion. */
	value = csas_get(context, "12345", 5, &value_len);
	snprintf(buf, value_len + 1, "%s", value);
	printf("value: %s\tlength: %d\n", buf, strlen(buf));
	free(value);
#endif

	return 0;

}
