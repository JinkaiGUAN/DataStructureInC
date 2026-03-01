//
// Created by Peter on 2026/2/1.
//

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "trycatch.h"
#include "err_code.h"
#include "uni_type.h"

#include "threadpool_api.h"

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
                pthread_exit((void*)M_COMMON_OK);
            }

            // 4. 从任务队列头部取出一个任务
            task = pool->taskQueue;
            if (task != NULL) {
                pool->taskQueue = task->next;
                // 如果队列为空，重置尾指针
                if (pool->taskQueue == NULL) {
                    pool->taskQueueTail = NULL;
                }
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

/**
 * 创建线程池
 * @param threadNum 工作线程数量， 建议不超过CPU核心数+2
 * @param threadPool 线程池对象指针
 * @return 见 err_code.h 定义， 成功返回0.
 */
U32 threadPoolCreate(U32 threadNum, ThreadPool* threadPool)
{
    TRY {
        if (threadPool == NULL) {
            return ERR_NULL_POINTER;
        }

        if (threadNum == 0) {
            return ERR_PARAMETER_WRONG;
        }

        // 初始化线程池结构
        threadPool->taskQueue = NULL;
        threadPool->taskQueueTail = NULL;
        threadPool->threadNum = threadNum;
        threadPool->isRunning = TRUE;

        // 初始化互斥锁
        if (pthread_mutex_init(&threadPool->mutex, NULL) != 0) {
            return ERR_MALLOC_FAILED;
        }

        // 初始化条件变量
        if (pthread_cond_init(&threadPool->cond, NULL) != 0) {
            pthread_mutex_destroy(&threadPool->mutex);
            return ERR_MALLOC_FAILED;
        }

        // 分配线程ID数组
        threadPool->threads = (pthread_t*)malloc(sizeof(pthread_t) * threadNum);
        if (threadPool->threads == NULL) {
            pthread_mutex_destroy(&threadPool->mutex);
            pthread_cond_destroy(&threadPool->cond);
            return ERR_MALLOC_FAILED;
        }

        // 创建工作线程
        for (U32 i = 0; i < threadNum; i++) {
            if (pthread_create(&threadPool->threads[i], NULL, (void*(*)(void*))workThread, threadPool) != 0) {
                // 创建失败，清理已创建的线程
                threadPool->isRunning = FALSE;
                pthread_cond_broadcast(&threadPool->cond);

                for (U32 j = 0; j < i; j++) {
                    pthread_join(threadPool->threads[j], NULL);
                }

                free(threadPool->threads);
                pthread_mutex_destroy(&threadPool->mutex);
                pthread_cond_destroy(&threadPool->cond);

                return ERR_MALLOC_FAILED;
            }
        }

        return M_COMMON_OK;
    }
    CATCH {
        LOG(LOG_LEVEL_ERROR, INFRA_ERR(), "");
        return INFRA_ERR();
    } FINALLY_OK(M_COMMON_OK);
}

/**
 * 向线程池中提交任务
 * @param threadPool 线程池对象指针
 * @param func  任务函数
 * @param arg 任务参数
 * @return 见 err_code.h 定义， 成功返回0.
 */
U32 threadPoolSubmit(ThreadPool* threadPool, taskFuncT func, void *arg)
{
    TRY {
        if (threadPool == NULL || func == NULL) {
            return ERR_NULL_POINTER;
        }

        if (threadPool->isRunning == FALSE) {
            return ERR_PARAMETER_WRONG;
        }

        // 创建新任务
        Task *task = (Task*)malloc(sizeof(Task));
        if (task == NULL) {
            return ERR_MALLOC_FAILED;
        }

        task->func = func;
        task->args = arg;
        task->next = NULL;

        // 加锁，保护任务队列
        pthread_mutex_lock(&threadPool->mutex);

        // 将任务添加到队列尾部
        if (threadPool->taskQueue == NULL) {
            threadPool->taskQueue = task;
            threadPool->taskQueueTail = task;
        } else {
            threadPool->taskQueueTail->next = task;
            threadPool->taskQueueTail = task;
        }

        // 解锁
        pthread_mutex_unlock(&threadPool->mutex);

        // 通知一个工作线程
        pthread_cond_signal(&threadPool->cond);

        return M_COMMON_OK;
    }
    CATCH {
        LOG(LOG_LEVEL_ERROR, INFRA_ERR(), "");
        return INFRA_ERR();
    } FINALLY_OK(M_COMMON_OK);
}

/**
 * 销毁线程池 （等待所有线程结束）
 * @param threadPool 线程池对象指针
 * @return 见 err_code.h 定义， 成功返回0.
 */
U32 threadPoolDestroy(ThreadPool* threadPool)
{
    TRY {
        if (threadPool == NULL) {
            return ERR_NULL_POINTER;
        }

        // 设置线程池停止标志
        pthread_mutex_lock(&threadPool->mutex);
        threadPool->isRunning = FALSE;
        pthread_mutex_unlock(&threadPool->mutex);

        // 通知所有工作线程
        pthread_cond_broadcast(&threadPool->cond);

        // 等待所有工作线程结束
        for (U32 i = 0; i < threadPool->threadNum; i++) {
            pthread_join(threadPool->threads[i], NULL);
        }

        // 清理任务队列中剩余的任务
        pthread_mutex_lock(&threadPool->mutex);
        Task *task = threadPool->taskQueue;
        while (task != NULL) {
            Task *temp = task;
            task = task->next;
            free(temp);
        }
        threadPool->taskQueue = NULL;
        threadPool->taskQueueTail = NULL;
        pthread_mutex_unlock(&threadPool->mutex);

        // 释放资源
        free(threadPool->threads);
        pthread_mutex_destroy(&threadPool->mutex);
        pthread_cond_destroy(&threadPool->cond);

        return M_COMMON_OK;
    }
    CATCH {
        LOG(LOG_LEVEL_ERROR, INFRA_ERR(), "");
        return INFRA_ERR();
    } FINALLY_OK(M_COMMON_OK);
}

// ============================================================================
// 测试代码区域
// ============================================================================

// 测试任务函数
static void* threadPoolTestTask(void* arg)
{
    int* taskId = (int*)arg;
    LOG(LOG_LEVEL_INFO, M_COMMON_OK, "Task %d is running", *taskId);
    sleep(1);
    LOG(LOG_LEVEL_INFO, M_COMMON_OK, "Task %d is completed", *taskId);
    free(taskId);
    return NULL;
}

// 线程池测试函数
U32 threadPoolTest()
{
    TRY {
        LOG(LOG_LEVEL_INFO, M_COMMON_OK, "=== Thread Pool Test ===");

        // 创建线程池
        ThreadPool pool;
        LOG(LOG_LEVEL_INFO, M_COMMON_OK, "Creating thread pool...");
        U32 ret = threadPoolCreate(4, &pool);
        if (ret != M_COMMON_OK) {
            LOG(LOG_LEVEL_ERROR, ret, "Failed to create thread pool");
            return ret;
        }
        LOG(LOG_LEVEL_INFO, M_COMMON_OK, "Thread pool created successfully");

        // 提交10个任务
        LOG(LOG_LEVEL_INFO, M_COMMON_OK, "Submitting tasks...");
        for (int i = 0; i < 10; i++) {
            int* taskId = (int*)malloc(sizeof(int));
            *taskId = i;
            ret = threadPoolSubmit(&pool, threadPoolTestTask, taskId);
            if (ret != M_COMMON_OK) {
                LOG(LOG_LEVEL_ERROR, ret, "Failed to submit task %d", i);
                free(taskId);
            }
        }
        LOG(LOG_LEVEL_INFO, M_COMMON_OK, "All tasks submitted");

        // 等待任务完成
        sleep(5);

        // 销毁线程池
        LOG(LOG_LEVEL_INFO, M_COMMON_OK, "Destroying thread pool...");
        ret = threadPoolDestroy(&pool);
        if (ret != M_COMMON_OK) {
            LOG(LOG_LEVEL_ERROR, ret, "Failed to destroy thread pool");
            return ret;
        }
        LOG(LOG_LEVEL_INFO, M_COMMON_OK, "Thread pool destroyed successfully");

        LOG(LOG_LEVEL_INFO, M_COMMON_OK, "Test completed");
        return M_COMMON_OK;
    }
    CATCH {
        LOG(LOG_LEVEL_ERROR, INFRA_ERR(), "");
        return INFRA_ERR();
    } FINALLY_OK(M_COMMON_OK);
}
