//
// Created by Peter on 2026/2/1.
//

#include <pthread.h>

#include "trycatch.h"

#include "threadpool.h"

// 工作线程入口
static U32 workThread(void *arg)
{
    TRY {
        ThreadPool *pool = (ThreadPool *)arg;
        Task *task = NULL;

        while (1) {
            // 1. 加锁，保护任务队列
            pthread_mutex_lock(&pool->mutex);

            // 2. 无任务且线程池运行中， 阻塞等待条件变量
            while (pool->taskQueue == NULL && pool->isRunning == TRUE) {
                pthread_cond_wait(&pool->cond, &pool->mutex);
            }

            // 3. 线程池停止任务， 退出线程
            if (pool->isRunning == FALSE && pool->taskQueue == NULL) {
                pthread_mutex_unlock(&pool->mutex);
                pthread_exit(NULL);
            }

            // 4. 从任务队列头部取出一个任务
            task = pool->taskQueue;
            if (task != NULL) {
                pool->taskQueue = task->next;
            }
            // 5. 解锁， 释放任务队列 （避免执行任务是持有锁， 影响并发）
            pthread_mutex_unlock(&pool->mutex);
            // 6. 执行任务（无锁环境， 提高效率）
            if (task != NULL) {
                task->func(task->args);
                free(task);
                task = NULL;
            }
        }

        return M_COMMON_OK;
    }
    CATCH {
        LOG(LOG_LEVEL_ERROR, INFRA_ERR(), "");
        return INFRA_ERR();
    } FINALLY_OK(M_COMMON_OK);
}

