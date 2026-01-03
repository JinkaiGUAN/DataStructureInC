#ifndef INFRA_LOG_H
#define INFRA_LOG_H

#include <stdio.h>
#include <stdarg.h>
#include "err_code.h"

#ifndef INFRA_ENABLE_DEBUG
#define INFRA_ENABLE_DEBUG 1
#endif

enum {
    LOG_LEVEL_INFO = 1,
    LOG_LEVEL_WARN = 2,
    LOG_LEVEL_ERROR = 3,
    LOG_LEVEL_DEBUG = 4
};

/* TRY/CATCH 抛错位置信息（线程局部），由 trycatch.h 进行维护 */
static _Thread_local const char *__infra_throw_file = NULL;
static _Thread_local const char *__infra_throw_func = NULL;
static _Thread_local int __infra_throw_line = 0;

/* 错误名由 err_code.h 提供的 ERR_NAME(code) 获取 */

static inline void infra_log(int level, int errcode,
                             const char *file, int line, const char *func,
                             const char *fmt, ...) {
    if (level == LOG_LEVEL_DEBUG && !INFRA_ENABLE_DEBUG) {
        return;
    }
    const char *lname = (level == LOG_LEVEL_INFO) ? "INFO" :
                        (level == LOG_LEVEL_WARN) ? "WARN" :
                        (level == LOG_LEVEL_ERROR) ? "ERROR" :
                        (level == LOG_LEVEL_DEBUG) ? "DEBUG" : "LOG";
    fprintf(stderr, "[%s] err=%s(%#x): ", lname, ERR_NAME(errcode), errcode);
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    if (__infra_throw_file) {
        fprintf(stderr, " (thrown at %s:%d %s)", __infra_throw_file, __infra_throw_line, __infra_throw_func);
    }
    fputc('\n', stderr);
}

#define LOG(level, errcode, fmt, ...) \
    infra_log((level), (errcode), __FILE__, __LINE__, __func__, (fmt), ##__VA_ARGS__)

#endif // INFRA_LOG_H
