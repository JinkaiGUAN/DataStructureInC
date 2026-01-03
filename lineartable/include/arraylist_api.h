//
// Created by Peter on 2026/1/2.
//

#ifndef DATASTRUCTUREINC_ARRAYLIST_API_H
#define DATASTRUCTUREINC_ARRAYLIST_API_H

#include "lineartable.h"

// 条件包含基础类型定义；若编辑器未配置包含路径，则提供安全回退
#if __has_include("uni_type.h")
#include "uni_type.h"
#else
#include <stdint.h>
typedef uint32_t U32;
#endif

#ifdef __cplusplus
extern "C" {
#endif


// 线性表-数组表示。
typedef struct arrayList {
    E *array;
    U32 capacity;
    U32 size;
} ArrayList;


U32 ArrayList_ArrayListTest(void);

#ifdef __cplusplus
};
#endif

#endif //DATASTRUCTUREINC_ARRAYLIST_API_H
