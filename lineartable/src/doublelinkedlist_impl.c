//
// Created by Peter on 2026/1/3.
//

#include "trycatch.h"

#include "doublelinkedlist_api.h"

U32 DoubleLinkedList_Init(DoubleLinkedList *self)
{
    TRY {
        THROW_IF(NULL == self, ERR_NULL_POINTER);

        self->head.next = NULL;
        self->head.prev = NULL;
        self->size = 0;

        return M_COMMON_OK;
    }
    CATCH {
        LOG(LOG_LEVEL_ERROR, INFRA_ERR(), "");
        return INFRA_ERR();
    } FINALLY_OK(M_COMMON_OK)
}

U32 DoubleLinkedList_InsertEle(DoubleLinkedList *self, E ele, U32 index)
{
    TRY {
        THROW_IF(NULL == self, ERR_NULL_POINTER);

        DoubleLinkedListNode *node = &self->head;
        while (--index) {
            node = node->next;
            THROW_IF(NULL == node, ERR_NULL_POINTER);
        }

        DoubleLinkedListNode *newNode = (DoubleLinkedListNode *) malloc(sizeof(DoubleLinkedListNode));
        THROW_IF(NULL == newNode, ERR_MALLOC_FAILED);

        newNode->val = ele;
        DoubleLinkedListNode *next = node->next;

        newNode->next = next;
        newNode->prev = node;
        node->next = newNode;
        if (NULL != next) {
            next->prev = newNode;
        }
        self->size++;

        return M_COMMON_OK;
    }
    CATCH {
        LOG(LOG_LEVEL_ERROR, INFRA_ERR(), "");
        return INFRA_ERR();
    } FINALLY_OK(M_COMMON_OK)
}

U32 DoubleLinkedList_DeleteEle(DoubleLinkedList *self, U32 index)
{
    TRY {
        THROW_IF(NULL == self, ERR_NULL_POINTER);
        DoubleLinkedListNode *head = &self->head;
        while (--index) {
            head = head->next;
            THROW_IF(NULL == head, ERR_NULL_POINTER);
        }
        THROW_IF(NULL == head->next, ERR_NULL_POINTER);

        DoubleLinkedListNode *removedNode = head->next;
        if (removedNode->next != NULL) {
            head->next = removedNode->next;
            removedNode->next->prev = head;
        } else {
            head->next = NULL;
        }
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

U32 DoubleLinkedList_PrintList(DoubleLinkedList *self)
{
    TRY {
        THROW_IF(NULL == self, ERR_NULL_POINTER);

        printf("\n==============\n");
        DoubleLinkedListNode *node = &self->head;
        node = node->next;
        while (NULL  != node) {
            printf("%d  ", node->val);
            node = node->next;
        }
        printf("\n size: %d", self->size);
        printf("\n==============\n");

        return M_COMMON_OK;
    }
    CATCH {
        LOG(LOG_LEVEL_ERROR, INFRA_ERR(), "");
        return INFRA_ERR();
    } FINALLY_OK(M_COMMON_OK)
}

U32 DoubleLinkedList_Test(void)
{
    TRY {
        DoubleLinkedList list = { 0 };
        EXEC(DoubleLinkedList_Init(&list));

        for (U32 i = 0; i < 4; i++) {
            EXEC(DoubleLinkedList_InsertEle(&list, (E)i, i + 1));
        }
        EXEC(DoubleLinkedList_PrintList(&list));

        EXEC(DoubleLinkedList_DeleteEle(&list, 2));
        EXEC(DoubleLinkedList_PrintList(&list));

        EXEC(DoubleLinkedList_DeleteEle(&list, 1));
        EXEC(DoubleLinkedList_PrintList(&list));


        return M_COMMON_OK;
    }
    CATCH {
        LOG(LOG_LEVEL_ERROR, INFRA_ERR(), "");
        return INFRA_ERR();
    } FINALLY_OK(M_COMMON_OK)
}

