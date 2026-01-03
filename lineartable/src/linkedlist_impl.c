//
// Created by Peter on 2026/1/3.
//


#include "uni_type.h"
#include "trycatch.h"

#include "linkedlist_api.h"


U32 LinkedList_Init(LinkedList *self)
{
    TRY {
        THROW_IF(NULL == self, M_COMMON_OK);
        self->head.next = NULL;

        return M_COMMON_OK;
    }
    CATCH {
        LOG(LOG_LEVEL_ERROR, INFRA_ERR(), "Init linkedList failed!");
        return INFRA_ERR();
    } FINALLY_OK(M_COMMON_OK)
}

U32 LinkedList_InsertEle(LinkedList *self, E ele, U32 index)
{
    TRY {
        THROW_IF(NULL == self, ERR_NULL_POINTER);
        THROW_IF(index <= 0 || index > self->size + 1, ERR_PARAMETER_WRONG);

        LinearTableNode *node = &self->head;
        while (--index) {
            node = node->next;
            if (NULL == node) {
                return ERR_PARAMETER_WRONG;
            }
        }

        LinearTableNode  *newNode = (LinearTableNode *) malloc(sizeof(LinearTableNode));
        newNode->val = ele;
        newNode->next = node->next;
        node->next = newNode;
        self->size++;

        return M_COMMON_OK;
    }
    CATCH {
        LOG(LOG_LEVEL_ERROR, INFRA_ERR(), "");
        return INFRA_ERR();
    } FINALLY_OK(M_COMMON_OK)
}

U32 LinkedList_DeleteEle(LinkedList *self, U32 index)
{
    TRY {
        THROW_IF(NULL == self, ERR_NULL_POINTER);
        THROW_IF(index <= 0 || index > self->size, ERR_PARAMETER_WRONG);

        LinearTableNode *node = &self->head;
        while (--index) {
            node = node->next;
            if (NULL == node) {
                return ERR_PARAMETER_WRONG;
            }
        }
        THROW_IF(NULL == node->next, ERR_NULL_POINTER);

        LinearTableNode *removedNode = node->next;
        node->next = node->next->next;
        free(removedNode);
        removedNode = NULL;
        self->size--;

        return M_COMMON_OK;
    }
    CATCH {
        LOG(LOG_LEVEL_ERROR, INFRA_ERR(), "");
        return INFRA_ERR();
    } FINALLY_OK(M_COMMON_OK)
}

U32 LinkedList_PrintList(LinkedList *self)
{
    TRY {
        THROW_IF(NULL == self, ERR_NULL_POINTER);

        LinearTableNode *node = &self->head;
        printf("\n===============\n");
        while (NULL != node->next) {
            printf("%d  ", node->val);
            node = node->next;
        }
        printf("\n Size: %u\n", self->size);
        printf("===============\n");

        return M_COMMON_OK;
    }
    CATCH {
        LOG(LOG_LEVEL_ERROR, INFRA_ERR(), "");
        return INFRA_ERR();
    } FINALLY_OK(M_COMMON_OK)
}

U32 LinkedList_GetSize(LinkedList *self, U32 *size)
{
    TRY {
        THROW_IF(NULL == self || NULL == size, ERR_NULL_POINTER);
        *size = self->size;

        return M_COMMON_OK;
    }
    CATCH {
        LOG(LOG_LEVEL_ERROR, INFRA_ERR(), "");
        return INFRA_ERR();
    } FINALLY_OK(M_COMMON_OK)
}

U32 LinkedList_Test(void)
{
    TRY {
        LinkedList list = { 0 };
        EXEC(LinkedList_Init(&list));

        EXEC(LinkedList_InsertEle(&list, (E)1, 1));
        EXEC(LinkedList_InsertEle(&list, (E)2, 2));
        EXEC(LinkedList_InsertEle(&list, (E)3, 3));

        EXEC(LinkedList_PrintList(&list));

        THROW_IF(LinkedList_InsertEle(&list, (E)4, 5) == M_COMMON_OK, ERR_TEST_FAILED);
        U32 size = 0;
        EXEC(LinkedList_GetSize(&list, &size));
        THROW_IF(size != 3, ERR_TEST_FAILED);

        EXEC(LinkedList_DeleteEle(&list, 2));
        EXEC(LinkedList_PrintList(&list));
        THROW_IF(LinkedList_DeleteEle(&list, 3) == M_COMMON_OK, ERR_TEST_FAILED);
        EXEC(LinkedList_GetSize(&list, &size));
        THROW_IF(size != 2, ERR_TEST_FAILED);

        return M_COMMON_OK;
    }
    CATCH {
        LOG(LOG_LEVEL_ERROR, INFRA_ERR(), "");
        return INFRA_ERR();
    } FINALLY_OK(M_COMMON_OK)
}
