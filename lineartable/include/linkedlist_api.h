//
// Created by Peter on 2026/1/3.
//

#ifndef DATASTRUCTUREINC_LINKEDLIST_API_H
#define DATASTRUCTUREINC_LINKEDLIST_API_H

#include "uni_type.h"

#include "lineartable.h"

UNITYPE_EXTERN_C_BEG

typedef struct node_ {
    E val;
    struct node_ *next;
} LinearTableNode;

typedef struct {
    LinearTableNode head;
    U32 size;
} LinkedList;

U32 LinkedList_Test(void);


UNITYPE_EXTERN_C_END


#endif //DATASTRUCTUREINC_LINKEDLIST_API_H
