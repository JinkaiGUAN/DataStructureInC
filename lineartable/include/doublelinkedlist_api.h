//
// Created by Peter on 2026/1/3.
//

#ifndef DATASTRUCTUREINC_DOUBLELINKEDLIST_API_H
#define DATASTRUCTUREINC_DOUBLELINKEDLIST_API_H

#include "uni_type.h"

#include "lineartable.h"

UNITYPE_EXTERN_C_BEG

typedef struct doubleLinkedListNode_ {
    E val;
    struct doubleLinkedListNode_ *prev;
    struct doubleLinkedListNode_ *next;
} DoubleLinkedListNode;

typedef struct doubleLinkedList {
    DoubleLinkedListNode head;
    U32 size;
} DoubleLinkedList;

U32 DoubleLinkedList_Test(void);

UNITYPE_EXTERN_C_END

#endif //DATASTRUCTUREINC_DOUBLELINKEDLIST_API_H
