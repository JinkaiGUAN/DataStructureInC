//
// Created by Peter on 2026/2/1.
//

#include <pthread.h>

#include "uni_type.h"
#include "err_code.h"

#ifndef DATASTRUCTUREINC_THREADPOOL_H
#define DATASTRUCTUREINC_THREADPOOL_H

/* 任务函数类型 */
typedef void* (*taskFuncT)(void *arg);

// 任务实体
typedef struct _Task {
    taskFuncT func;         // 任务函数
    void *args;             // 任务函数参数
    struct _Task* next;     // 下一个任务
} Task;

// 线程池定义
typedef struct _ThreadPool {
    Task *taskQueue;        // 任务队列（链表头节点）
    Task *taskQueueTail;    // 任务队列（链表尾节点）
    U32 threadNum;          // 工作线程数量
    BOOL isRunning;         // 线程池状态， TRUE 表示运行， FALSE 表示停止。
    pthread_t* threads;     // 工作线程ID数组
    pthread_mutex_t mutex;  // 保护任务队列的互斥锁
    pthread_cond_t cond;    // 唤醒工作线程的条件变更
} ThreadPool;


/**
 * 创建线程
 * @param threadNum 工作线程数量， 建议不超过CPU核心数+2
 * @return 见 err_code.h 定义， 成功返回0.
 */
U32 threadPoolCreate(U32 threadNum, ThreadPool* threadPool);

/**
 * 向线程池中提交任务
 * @param threadPool 线程池对象指针
 * @param func  任务函数
 * @param arg 任务参数
 * @return 见 err_code.h 定义， 成功返回0.
 */
U32 threadPoolSubmit(ThreadPool* threadPool, taskFuncT func, void *arg);

/**
 * 销毁线程池 （等待所有线程结束）
 * @param threadPool
 * @return 见 err_code.h 定义， 成功返回0.
 */
U32 threadPoolDestroy(ThreadPool* threadPool);

/**
 * 线程池测试函数
 * @return 见 err_code.h 定义， 成功返回0.
 */
U32 threadPoolTest();


#endif //DATASTRUCTUREINC_THREADPOOL_H
