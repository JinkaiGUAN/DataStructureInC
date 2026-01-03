#ifndef INFRA_TRYCATCH_H
#define INFRA_TRYCATCH_H

#include <setjmp.h>
#include "log.h"

#define INFRA_OK 0

/*
 * TRY/CATCH 使用方式：
 *   TRY { ... } CATCH { ... } [FINALLY_OK(okval)]
 * 语义：在 TRY 内部，使用 EXEC(expr)、THROW_IF(cond, code)、THROW(code)
 * 出错时将立即跳转进入 CATCH（通过 longjmp 实现），不依赖循环或 break。
 * 不需要也不允许在用户代码中显式声明错误变量；宏内部管理错误码。
 */

/* 错误码访问（TRY/CATCH 序列中的错误码） */
#define INFRA_ERR() (__infra_errcode)

/* 开启 TRY 块：声明错误码与跳转缓冲，进入“正常路径”；同时清理抛错位置信息 */
#define TRY jmp_buf __infra_jmp; int __infra_errcode = INFRA_OK; \
             __infra_throw_file = NULL; \
             __infra_throw_func = NULL; \
             __infra_throw_line = 0; \
             if (setjmp(__infra_jmp) == 0)

/* 开启 CATCH 块：当错误码非 OK 时进入“异常路径” */
#define CATCH else if (__infra_errcode != INFRA_OK)

/* 抛错：设置错误码并直接跳转进入 CATCH */
#define THROW(code) do { \
    __infra_errcode = (code); \
    __infra_throw_file = __FILE__; \
    __infra_throw_func = __func__; \
    __infra_throw_line = __LINE__; \
    longjmp(__infra_jmp, 1); \
} while (0)

/* 执行表达式：返回值为 0 视为 OK，否则设为错误码并直接跳转进入 CATCH */
#define EXEC(expr)  do { \
    int __infra_ret = (expr); \
    if (__infra_ret != INFRA_OK) { \
        __infra_errcode = __infra_ret; \
        __infra_throw_file = __FILE__; \
        __infra_throw_func = __func__; \
        __infra_throw_line = __LINE__; \
        longjmp(__infra_jmp, 1); \
    } \
} while (0)

/* 条件不满足则抛错：当 cond 为真时设置错误码并直接跳转进入 CATCH */
#define THROW_IF(cond, code) do { \
    if (cond) { \
        __infra_errcode = (code); \
        __infra_throw_file = __FILE__; \
        __infra_throw_func = __func__; \
        __infra_throw_line = __LINE__; \
        longjmp(__infra_jmp, 1); \
    } \
} while (0)

/* 在 CATCH 之后使用：若未出错（ERR==OK），则作为成功路径返回指定值 */
#define FINALLY_OK(okval) else { return (okval); }

/* 抛错位置由 log.h 中的线程局部变量维护，在 TRY 中清理，在 THROW/EXEC/THROW_IF 中设置 */

#endif // INFRA_TRYCATCH_H
