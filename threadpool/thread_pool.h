#ifndef _THREAD_POOL_H_INCLUDED_
#define _THREAD_POOL_H_INCLUDED_


#include "thread.h"

#define DEFAULT_THREADS_NUM 4
#define DEFAULT_QUEUE_NUM  65535


typedef unsigned long         atomic_uint_t;
typedef struct thread_task_s  thread_task_t;
typedef struct thread_pool_s  thread_pool_t;


struct thread_task_s {
    thread_task_t       *next;//下一个任务
    uint_t               id;       // 任务标识
    void                *ctx;     //上下文 用于保存任务的参数数据
    void               (*handler)(void *data);//处理函数
};

typedef struct {
    thread_task_t        *first;
    thread_task_t        **last;
} thread_pool_queue_t;

#define thread_pool_queue_init(q)                                         \
    (q)->first = NULL;                                                    \
    (q)->last = &(q)->first


struct thread_pool_s {
    pthread_mutex_t        mtx;
    thread_pool_queue_t   queue;
    int_t                 waiting;  //等待的数量
    pthread_cond_t         cond;

    char                  *name;     //线程池的名字
    uint_t                threads;   //线程的数量
    int_t                 max_queue;  //最大队列长度
};

thread_task_t *thread_task_alloc(size_t size);
int_t thread_task_post(thread_pool_t *tp, thread_task_t *task);
thread_pool_t* thread_pool_init();
void thread_pool_destroy(thread_pool_t *tp);


#endif /* _THREAD_POOL_H_INCLUDED_ */
