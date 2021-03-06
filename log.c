#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "log.h"

// Name of environment variable to control run-time logging.
#define LOG_LEVEL_ENV "LOG_LEVEL"

static LogInfo log_info = { LOG_LEVEL_COMPILE_TIME, -1 };

static const char* log_level_name[LOG_LEVEL_LAST] = {
    "DEBUG",
    "INFO",
    "WARNING",
    "ERROR",
};

static const char* log_level_label[LOG_LEVEL_LAST] = {
    "DBG",
    "INF",
    "WRN",
    "ERR",
};

static int log_get_runtime_level(void) {
    if (log_info.level_run_time < 0) {
        const char* str = getenv(LOG_LEVEL_ENV);
        int val = -1;
        if (str) {
            // try with log level name / label
            for (int j = 0; val < 0 && j < LOG_LEVEL_LAST; ++j) {
                if (strcmp(str, log_level_name[j]) == 0 ||
                    strcmp(str, log_level_label[j]) == 0) {
                    val = j;
                    break;
                }
            }
            // try with log level as a number
            if (val < 0) {
                val = strtol(str, 0, 10);
                if (val == 0 && errno == EINVAL) {
                    val = -1;
                }
            }
        }
        log_info.level_run_time = val < 0 ? LOG_LEVEL_COMPILE_TIME : val;
    }
    return log_info.level_run_time;
}

static void log_print(int level, const char* fmt, va_list ap) {
    int saved_errno = errno;

    time_t seconds = time(0);
    struct tm* local = localtime(&seconds);

    pid_t pid = getpid();

    fprintf(stderr, "%%%.3s %04d%02d%02d %02d%02d%02d %5ld | ",
            log_level_label[level],
            local->tm_year + 1900, local->tm_mon + 1, local->tm_mday,
            local->tm_hour, local->tm_min, local->tm_sec,
            (long int) pid);
    if (level >= LOG_LEVEL_WARNING) {
        if (saved_errno) {
            fprintf(stderr, "(%d) %s | ", saved_errno, strerror(saved_errno));
        } else {
            fprintf(stderr, "%s | ", strerror(saved_errno));
        }
    }
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
}

void log_print_debug(const char* fmt, ...) {
    if (log_get_runtime_level() > LOG_LEVEL_DEBUG) {
        return;
    }
    va_list ap;
    va_start(ap, fmt);
    log_print(LOG_LEVEL_DEBUG, fmt, ap);
    va_end(ap);
}

void log_print_info(const char* fmt, ...) {
    if (log_get_runtime_level() > LOG_LEVEL_INFO) {
        return;
    }
    va_list ap;
    va_start(ap, fmt);
    log_print(LOG_LEVEL_INFO , fmt, ap);
    va_end(ap);
}

void log_print_warning(const char* fmt, ...) {
    if (log_get_runtime_level() > LOG_LEVEL_WARNING) {
        return;
    }
    va_list ap;
    va_start(ap, fmt);
    log_print(LOG_LEVEL_WARNING , fmt, ap);
    va_end(ap);
}

void log_print_error(const char* fmt, ...) {
    if (log_get_runtime_level() > LOG_LEVEL_ERROR) {
        return;
    }
    va_list ap;
    va_start(ap, fmt);
    log_print(LOG_LEVEL_ERROR, fmt, ap);
    va_end(ap);

    abort();
}

const LogInfo* log_get_info(void) {
    return &log_info;
}
