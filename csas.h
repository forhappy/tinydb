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

#ifndef __CSAS_H__
#define __CSAS_H__

#include <stdlib.h>
#include <assert.h>

typedef struct engine_base_s_ engine_base_t;
typedef struct engine_operation_s_ engine_operation_t;
typedef struct csas_context_s_ csas_context_t;

/*
 * 通用存储服务
 */
struct csas_context_s_ {
	engine_base_t  *engine;
	csas_context_t *next;
	/* more stuff here... */
};

/*
 * 存储引擎
 */
struct engine_base_s_ {
	char               *name;        // 数据库名称      
	size_t             version;      // 数据库版本
	engine_operation_t *engine_ops;  // 数据库操作集合
	/* more stuff about storage engine base here... */
};

/*
 * 数据库操作集合
 */
struct engine_operation_s_ {
	int (*put)(engine_base_t *engine,
			   const char *key,
			   size_t key_len,
			   const char *value,
			   size_t value_len);

	char * (*get)(engine_base_t *engine,
			      const char * key,
				  size_t key_len,
			      size_t *value_len);

	int (*del)(engine_base_t *engine,
			   const char *key,
			   size_t key_len);
	
	void (*quit)(engine_base_t *engine);
};

/*
 * 初始化CSAS服务
 */
csas_context_t * csas_init(engine_base_t *engine);

/*
 * CSAS - PUT服务
 */
int csas_put(csas_context_t *context,
		     const char *key,
			 size_t key_len,
		     const char *value,
			 size_t value_len);

/*
 * CSAS - GET服务
 */
char * csas_get(csas_context_t *context,
		        const char *key,
				size_t key_len,
		        size_t *value_len);

/*
 * CSAS - DELETE服务
 */
int csas_delete(csas_context_t *context,
		        const char *key,
				size_t key_len);

/*
 * 清除CSAS服务
 */
void csas_destory(csas_context_t *context);

#endif /* __CSAS_H__ */
