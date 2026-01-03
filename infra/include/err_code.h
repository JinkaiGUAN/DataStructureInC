//
// Created by Peter on 2026/1/2.
//

#ifndef DATASTRUCTUREINC_ERR_CODE_H
#define DATASTRUCTUREINC_ERR_CODE_H

#ifdef __cplusplus
extern "C" {
#endif

/* 错误码域：高 16 位为域 ID，低 16 位为域内编号 */
#define ERR_DOMAIN_BASE   (0x21f)
#define ERR_BASE(x)       ((ERR_DOMAIN_BASE) << 16 | (x))

/* 使用 X-Macro 定义错误码，以便同时生成枚举与名称映射 */
#define ERR_CODES(X)                             \
    X(M_COMMON_OK,           0)                  \
    X(ERR_NULL_POINTER,      ERR_BASE(0))        \
    X(ERR_PARAMETER_WRONG,   ERR_BASE(1))        \
    X(ERR_MALLOC_FAILED,     ERR_BASE(2))        \
    X(ERR_EMPTY_LIST,        ERR_BASE(3))

/* 错误码枚举（保持现有值不变） */
typedef enum ErrCode {
#define X(name, val) name = (val),
    ERR_CODES(X)
#undef X
} ErrCode;

/* 错误名映射：将错误码转换为可读名称 */
static inline const char *err_name(int code) {
    switch (code) {
#define X(name, val) case name: return #name;
        ERR_CODES(X)
#undef X
        default: return "UNKNOWN_ERROR";
    }
}

/* 便捷宏：获取错误名 */
#define ERR_NAME(code) (err_name((code)))

#ifdef __cplusplus
}
#endif

#endif //DATASTRUCTUREINC_ERR_CODE_H
