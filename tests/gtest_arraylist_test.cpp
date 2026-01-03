// GoogleTest-based test for ArrayList
#include <gtest/gtest.h>

// 直接依赖公共接口与错误码；包含目录由 lineartable/infra 目标传播
#include "arraylist_api.h"
#include "err_code.h"


TEST(ArrayList, BasicScenario)
{
    // 业务接口返回 M_COMMON_OK 视为通过
    U32 rc = ArrayList_ArrayListTest();
    EXPECT_EQ(rc, M_COMMON_OK);
}