//
// Created by Peter on 2026/1/2.
//

#ifndef DATASTRUCTUREINC_UNI_TYPE_H
#define DATASTRUCTUREINC_UNI_TYPE_H

#include "stdlib.h"

#ifdef _cplusplus
extern "C" {
#endif

    typedef unsigned int U32;
    typedef int S32;

    typedef unsigned int BOOL;

    #define TRUE  (1)
    #define FALSE (0)

#ifdef __cplusplus
#define UNITYPE_EXTERN_C_BEG  extern "C" {
    #define UNITYPE_EXTERN_C_END    }
#else
#define UNITYPE_EXTERN_C_BEG
#define UNITYPE_EXTERN_C_END
#endif

#ifdef _cplusplus
};
#endif

#endif //DATASTRUCTUREINC_UNI_TYPE_H
