// GoogleTest-based test for ThreadPool
#include <gtest/gtest.h>
#include <unistd.h>

extern "C" {
#include "threadpool_api.h"
#include "err_code.h"
#include "log.h"
}

static volatile int debug_breakpoint_flag = 0;

static void* threadPoolTestTask(void* arg)
{
    int* taskId = (int*)arg;
    LOG(LOG_LEVEL_INFO, M_COMMON_OK, "Task %d is running", *taskId);
    sleep(1);
    LOG(LOG_LEVEL_INFO, M_COMMON_OK, "Task %d is completed", *taskId);
    free(taskId);
    return NULL;
}

TEST(ThreadPoolTest, CreateThreadPool)
{
    debug_breakpoint_flag = 1;

    LOG(LOG_LEVEL_INFO, M_COMMON_OK, "=== Thread Pool Create Test ===");

    ThreadPool pool;
    LOG(LOG_LEVEL_INFO, M_COMMON_OK, "Creating thread pool...");
    U32 ret = threadPoolCreate(4, &pool);
    EXPECT_EQ(ret, M_COMMON_OK);
    LOG(LOG_LEVEL_INFO, M_COMMON_OK, "Thread pool created successfully");
}

TEST(ThreadPoolTest, SubmitTasks)
{
    debug_breakpoint_flag = 2;

    LOG(LOG_LEVEL_INFO, M_COMMON_OK, "=== Thread Pool Submit Tasks Test ===");

    ThreadPool pool;
    LOG(LOG_LEVEL_INFO, M_COMMON_OK, "Creating thread pool...");
    U32 ret = threadPoolCreate(4, &pool);
    EXPECT_EQ(ret, M_COMMON_OK);
    LOG(LOG_LEVEL_INFO, M_COMMON_OK, "Thread pool created successfully");

    LOG(LOG_LEVEL_INFO, M_COMMON_OK, "Submitting tasks...");
    for (int i = 0; i < 10; i++) {
        int* taskId = (int*)malloc(sizeof(int));
        *taskId = i;
        ret = threadPoolSubmit(&pool, threadPoolTestTask, taskId);
        EXPECT_EQ(ret, M_COMMON_OK);
    }
    LOG(LOG_LEVEL_INFO, M_COMMON_OK, "All tasks submitted");

    sleep(5);

    LOG(LOG_LEVEL_INFO, M_COMMON_OK, "Destroying thread pool...");
    ret = threadPoolDestroy(&pool);
    EXPECT_EQ(ret, M_COMMON_OK);
    LOG(LOG_LEVEL_INFO, M_COMMON_OK, "Thread pool destroyed successfully");
}

TEST(ThreadPoolTest, FullTest)
{
    debug_breakpoint_flag = 3;

    LOG(LOG_LEVEL_INFO, M_COMMON_OK, "=== Thread Pool Full Test ===");

    ThreadPool pool;
    LOG(LOG_LEVEL_INFO, M_COMMON_OK, "Creating thread pool...");
    U32 ret = threadPoolCreate(4, &pool);
    EXPECT_EQ(ret, M_COMMON_OK);
    LOG(LOG_LEVEL_INFO, M_COMMON_OK, "Thread pool created successfully");

    LOG(LOG_LEVEL_INFO, M_COMMON_OK, "Submitting tasks...");
    for (int i = 0; i < 10; i++) {
        int* taskId = (int*)malloc(sizeof(int));
        *taskId = i;
        ret = threadPoolSubmit(&pool, threadPoolTestTask, taskId);
        EXPECT_EQ(ret, M_COMMON_OK);
    }
    LOG(LOG_LEVEL_INFO, M_COMMON_OK, "All tasks submitted");

    sleep(5);

    LOG(LOG_LEVEL_INFO, M_COMMON_OK, "Destroying thread pool...");
    ret = threadPoolDestroy(&pool);
    EXPECT_EQ(ret, M_COMMON_OK);
    LOG(LOG_LEVEL_INFO, M_COMMON_OK, "Thread pool destroyed successfully");

    LOG(LOG_LEVEL_INFO, M_COMMON_OK, "Full test completed");
}
