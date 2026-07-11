
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>

#include "pthread_wrapper.h"

#if defined(__APPLE__) || defined(__MACH__)
#include <errno.h>
static int pthread_mutex_timedlock(pthread_mutex_t* mtx, const struct timespec* abstime)
{
    if (!mtx || !abstime) return EINVAL;

    int ret;
    struct timespec now;

    while (1)
    {
        ret = pthread_mutex_trylock(mtx);
        if (ret == 0)
        {
            return 0;
        }
        else if (ret != EBUSY)
        {
            return ret;
        }

        // 获取当前时间，判断是否超时
        clock_gettime(CLOCK_REALTIME, &now);
        if (now.tv_sec > abstime->tv_sec ||
            (now.tv_sec == abstime->tv_sec && now.tv_nsec >= abstime->tv_nsec))
        {
            return ETIMEDOUT;
        }

        // 短暂休眠，避免CPU空转
        struct timespec ts = {0, 5000000};
        nanosleep(&ts, NULL);
    }
}
#endif


uint64_t get_tid_wrap(void)
{
    return (uint64_t)pthread_self();
}

void ms_sleep(uint32_t ms)
{
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, NULL);
}


// ===================== 线程创建/回收 完整实现 =====================
pthread_wrap_err_t thread_create_wrap(pthread_t *tid, const thread_cfg_t *cfg, thread_entry_t entry, void *arg)
{
    if (!tid || !cfg || !entry) {
        return PWRAP_ERR_MEM;
    }

    pthread_attr_t attr;
    int ret = pthread_attr_init(&attr);
    if (ret != 0) {
        return PWRAP_ERR_CREATE_THREAD;
    }

    if (cfg->stack_size > 0) {
        pthread_attr_setstacksize(&attr, cfg->stack_size);
    }

    if (cfg->detach) {
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    } else {
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    }

    if (cfg->sched_policy == SCHED_FIFO || cfg->sched_policy == SCHED_RR) {
        struct sched_param sp;
        sp.sched_priority = cfg->priority;
        pthread_attr_setschedpolicy(&attr, cfg->sched_policy);
        pthread_attr_setschedparam(&attr, &sp);
        pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    }

    ret = pthread_create(tid, &attr, entry, arg);
    pthread_attr_destroy(&attr);
    if (ret != 0) {
        return PWRAP_ERR_CREATE_THREAD;
    }

#if defined(__linux__)
    if (cfg->cpu_mask > 0) {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(cfg->cpu_mask, &cpuset);
        pthread_setaffinity_np(*tid, sizeof(cpu_set_t), &cpuset);
    }
#endif

    return PWRAP_OK;
}

pthread_wrap_err_t thread_join_wrap(pthread_t tid, void **ret)
{
    if (pthread_join(tid, ret) == 0) {
        return PWRAP_OK;
    } else {
        return PWRAP_ERR_CREATE_THREAD;
    }
}

// ===================== 超时互斥锁 完整实现 =====================
pthread_wrap_err_t mutex_init_wrap(mutex_wrap_t* lock, bool prio_inherit)
{
    if (!lock) {
        return PWRAP_ERR_MEM;
    }

    int ret = pthread_mutexattr_init(&lock->attr);
    if (ret != 0) {
        return PWRAP_ERR_CREATE_THREAD;
    }


#if defined(__linux__)
    if (prio_inherit) {
        pthread_mutexattr_setprotocol(&lock->attr, PTHREAD_PRIO_INHERIT);
    }
#endif
    ret = pthread_mutex_init(&lock->mtx, &lock->attr);
    pthread_mutexattr_destroy(&lock->attr);

    return ret == 0 ? PWRAP_OK : PWRAP_ERR_LOCK_FAIL;
}


pthread_wrap_err_t mutex_lock_timeout(mutex_wrap_t* lock, uint32_t ms)
{
    if (!lock) return PWRAP_ERR_MEM;

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += ms / 1000;
    ts.tv_nsec += (ms % 1000) * 1000000;

    // 纳秒进位处理
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
    }

    int ret = pthread_mutex_timedlock(&lock->mtx, &ts);
    if (ret == ETIMEDOUT) return PWRAP_ERR_LOCK_TIMEOUT;
    return ret == 0 ? PWRAP_OK : PWRAP_ERR_LOCK_FAIL;
}

void mutex_unlock_wrap(mutex_wrap_t* lock)
{
    if (lock) pthread_mutex_unlock(&lock->mtx);
}

void mutex_destroy_wrap(mutex_wrap_t* lock)
{
    if (lock) pthread_mutex_destroy(&lock->mtx);
}

pthread_wrap_err_t cond_init_wrap(cond_wrap_t* cond)
{
    if (!cond) return PWRAP_ERR_MEM;
    int r = pthread_cond_init(&cond->cond, NULL);
    return r == 0 ? PWRAP_OK : PWRAP_ERR_WAIT_TIMEOUT;
}


pthread_wrap_err_t cond_wait_timeout(cond_wrap_t* cond, mutex_wrap_t* lock, uint32_t ms)
{
    if (!cond || !lock) return PWRAP_ERR_MEM;

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += ms / 1000;
    ts.tv_nsec += (ms % 1000) * 1000000;

    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
    }

    int ret = pthread_cond_timedwait(&cond->cond, &lock->mtx, &ts);
    if (ret == ETIMEDOUT) return PWRAP_ERR_WAIT_TIMEOUT;
    return ret == 0 ? PWRAP_OK : PWRAP_ERR_WAIT_TIMEOUT;
}


void cond_signal_wrap(cond_wrap_t* cond)
{
    if (cond) pthread_cond_signal(&cond->cond);
}

void cond_broadcast_wrap(cond_wrap_t* cond)
{
    if (cond) pthread_cond_broadcast(&cond->cond);
}

void cond_destroy_wrap(cond_wrap_t* cond)
{
    if (cond) pthread_cond_destroy(&cond->cond);
}

void* normal_thread(void* arg)
{
    while(1)
    {
        printf("普通业务线程运行...\n");
        ms_sleep(1000);
    }
    return NULL;
}

int main(void)
{
    pthread_t tid;
    // 普通线程配置：默认栈、分时调度、不绑定CPU、可join
    thread_cfg_t cfg = {
            .stack_size = 0,
            .sched_policy = SCHED_OTHER,
            .priority = 0,
            .cpu_mask = 0,
            .detach = false
    };

    thread_create_wrap(&tid, &cfg, normal_thread, NULL);

    // 主线程保活，不杀死子线程
    pthread_exit(NULL);
    return 0;
}