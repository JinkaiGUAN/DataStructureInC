#include <stdio.h>
// 使用项目内相对路径，避免 IDE 未载入 CMake 时的解析问题
#include "log.h"
#include "trycatch.h"

#include "arraylist_api.h"
#include "linkedlist_api.h"

int main()
{
    // infra TRY/CATCH/EXEC/THROW_IF 使用示例（TRY { } CATCH { } 形式）
    TRY {
        //EXEC(ArrayList_ArrayListTest());
        EXEC(LinkedList_Test());
        return 0;
    } CATCH {
        LOG(LOG_LEVEL_ERROR, INFRA_ERR(), "caught error, handling...");
        return 1;
    }
}
