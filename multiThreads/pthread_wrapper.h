
#ifndef __PTHREAD_WRAPPER_H__
#define __PTHREAD_WRAPPER_H__

#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>

#include <pthread.h>

// 统一错误码
typedef enum {
    PWRAP_OK = 0,
    PWRAP_ERR_CREATE_THREAD,
    PWRAP_ERR_LOCK_TIMEOUT,
    PWRAP_ERR_LOCK_FAIL,
    PWRAP_ERR_WAIT_TIMEOUT,
    PWRAP_ERR_MEM,
} pthread_wrap_err_t;


// 线程配置
typedef struct {
    size_t stack_size;          // 自定义栈大小，0=默认8MB，嵌入式建议4096/8192
    int sched_policy;            // SCHED_OTHER/SCHED_FIFO/SCHED_RR
    int priority;                // 实时优先级 1~99
    int cpu_mask;                // CPU绑定核心，0=不绑定
    bool detach;                 // true=分离线程(临时)，false=可回收(常驻)
} thread_cfg_t;

// 线程函数标准入口
typedef void* (*thread_entry_t)(void* arg);


// --------------- 线程操作接口 ---------------
pthread_wrap_err_t thread_create_wrap(pthread_t *tid, const thread_cfg_t *cfg, thread_entry_t entry, void *arg);

pthread_wrap_err_t thread_join_wrap(pthread_t tid, void **ret);

// --------------- 超时互斥锁接口 ---------------

// 超时互斥锁
typedef struct {
    pthread_mutex_t mtx;
    pthread_mutexattr_t attr;
    bool is_prio_inherit; // 是否开启优先级继承（实时工控必备）
} mutex_wrap_t;

pthread_wrap_err_t mutex_init_wrap(mutex_wrap_t *lock, bool prio_inherit);
pthread_wrap_err_t mutex_lock_timeout(mutex_wrap_t *lock, uint32_t ms);
void mutex_unlock_wrap(mutex_wrap_t *lock);
void mutex_destory_wrap(mutex_wrap_t *lock);

// --------------- 超时条件变量接口 ---------------
// 超时变量
typedef struct {
    pthread_cond_t cond;
} cond_wrap_t;

pthread_wrap_err_t cond_init_wrap(cond_wrap_t *cond);
pthread_wrap_err_t cond_wait_timeout(cond_wrap_t *cond, mutex_wrap_t *lock, uint32_t ms);
void cond_signal_wrap(cond_wrap_t *cond);
void cond_broadcast_wrap(cond_wrap_t *cond);
void cond_destroy_wrap(cond_wrap_t *cond);

// --------------- 工具接口 ---------------
uint64_t get_tid_wrap(void);
void ms_sleep(uint32_t ms);

#endif