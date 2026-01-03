//
// Created by Peter on 2026/1/2.
//

// 依赖 CMake 的包含目录传播：infra/include 与 lineartable/include
#include <string.h>

#include "log.h"
#include "trycatch.h"
#include "uni_type.h"
#include "err_code.h"
#include "arraylist_api.h"

U32 ArrayList_InitArrayList(ArrayList *self)
{
    TRY {
        THROW_IF(self == NULL, ERR_NULL_POINTER);
        self->capacity = 10;
        self->size = 0;
        self->array = (E *) malloc(sizeof(E) * self->capacity);
        THROW_IF(self->array == NULL, ERR_MALLOC_FAILED);
        memset_s(self->array, sizeof(E) * self->capacity, 0, sizeof(E) * self->capacity);
        return M_COMMON_OK;
    }
    CATCH {
        LOG(LOG_LEVEL_ERROR, INFRA_ERR(), "arrayList init failed!");
        return INFRA_ERR();
    } FINALLY_OK(M_COMMON_OK)
}

/*
 * index: Indicates the element position in the array, starting from 1.
 * */
U32 ArrayList_InsertEle(ArrayList *self, E ele, U32 index)
{
    TRY {
        THROW_IF(self == NULL, ERR_NULL_POINTER);
        THROW_IF(index <= 0 || index > self->size + 1, ERR_PARAMETER_WRONG);

        if (self->size >= self->capacity) {
            U32 newCapacity = self->capacity + (self->capacity >> 1);
            E *newArray = (E *)realloc(self->array, newCapacity);
            THROW_IF(newArray == NULL, ERR_MALLOC_FAILED);
            self->array = newArray;
            self->capacity = newCapacity;
        }

        for (S32 idx = (S32)self->size; idx > index - 1; idx--) {
            self->array[idx] = self->array[idx - 1];
        }
        self->array[index - 1] = ele;
        self->size++;
        return M_COMMON_OK;
    }
    CATCH {
        LOG(LOG_LEVEL_ERROR, INFRA_ERR(), "Insert element failed!");
        return INFRA_ERR();
    } FINALLY_OK(M_COMMON_OK)
}

U32 ArrayList_DeleteEle(ArrayList *self, U32 index)
{
    TRY {
        THROW_IF(self == NULL, ERR_NULL_POINTER);
        THROW_IF(self->size == 0, ERR_EMPTY_LIST);
        THROW_IF(index <= 0 || index > self->size, ERR_PARAMETER_WRONG);

        for (U32 idx = index; idx < self->size; idx++) {
            self->array[idx - 1] = self->array[idx];
        }
        self->size--;

        return M_COMMON_OK;
    }
    CATCH {
        LOG(LOG_LEVEL_ERROR, INFRA_ERR(), "Delete element failed!");
        return INFRA_ERR();
    } FINALLY_OK(M_COMMON_OK)
}

U32 ArrayList_GetArraySize(ArrayList *self, U32 *size)
{
    TRY {
        THROW_IF(NULL == self || NULL == size, ERR_NULL_POINTER);
        *size = self->size;
        return M_COMMON_OK;
    }
    CATCH {
        LOG(LOG_LEVEL_ERROR, INFRA_ERR(), "Get the size of array failed!");
        return INFRA_ERR();
    } FINALLY_OK(M_COMMON_OK)
}

U32 ArrayList_GetEle(ArrayList *self, U32 index, E *ele)
{
    TRY {
        THROW_IF(NULL == self || NULL == ele, ERR_NULL_POINTER);
        THROW_IF(index <= 0 || index > self->size, ERR_PARAMETER_WRONG);

        *ele = self->array[index - 1];
        return M_COMMON_OK;
    }
    CATCH {
        LOG(LOG_LEVEL_ERROR, INFRA_ERR(), "Get the ele of array failed!");
        return INFRA_ERR();
    } FINALLY_OK(M_COMMON_OK)
}

U32 ArrayList_PrintArray(ArrayList *self)
{
    TRY {
        THROW_IF(NULL == self, ERR_NULL_POINTER);

        printf("\n========\n");
        for (U32 idx = 0; idx < self->size; idx++) {
            printf("%d  ", self->array[idx]);
        }
        printf("\n Capacity: %u, size: %u\n", self->capacity, self->size);
        printf("========\n");

        return M_COMMON_OK;
    }
    CATCH {
        LOG(LOG_LEVEL_ERROR, INFRA_ERR(), "Print the ele of array failed!");
        return INFRA_ERR();
    } FINALLY_OK(M_COMMON_OK)
}

U32 ArrayList_DestroyArray(ArrayList *self)
{
    TRY {
        THROW_IF(NULL == self, ERR_NULL_POINTER);

        if (NULL != self->array) {
            free(self->array);
            self->array = NULL;
        }
        self->size = 0;
        self->capacity = 0;

        return M_COMMON_OK;
    }
    CATCH {
        LOG(LOG_LEVEL_ERROR, INFRA_ERR(), "Print the ele of array failed!");
        return INFRA_ERR();
    } FINALLY_OK(M_COMMON_OK)
}

U32 ArrayList_ArrayListTest(void)
{
    TRY {
        ArrayList list = { 0 };
        EXEC(ArrayList_InitArrayList(&list));
        //EXEC(ArrayList_InsertEle(&list, (E)222, 0));
        // 测试临界点
        for (U32 i = 0; i < list.capacity; i++) {
            EXEC(ArrayList_InsertEle(&list, (E)i, i + 1));
        }
        EXEC(ArrayList_PrintArray(&list));

        EXEC(ArrayList_DestroyArray(&list));
        EXEC(ArrayList_PrintArray(&list));
        return M_COMMON_OK;
    }
    CATCH {
        LOG(LOG_LEVEL_ERROR, INFRA_ERR(), "array list test failed!");
        return INFRA_ERR();
    } FINALLY_OK(M_COMMON_OK)
}


