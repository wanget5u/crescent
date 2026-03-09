#include "tools.h"

#define COLOR_RESET  "\x1b[0m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_RED    "\x1b[31m"
#define COLOR_CYAN   "\x1b[35m"

void log_time(void) {
    struct timespec time_spec;
    timespec_get(&time_spec, TIME_UTC);
    struct tm* time_info = localtime(&time_spec.tv_sec);
    i32 milliseconds = time_spec.tv_nsec / 1000000;
    printf(COLOR_CYAN "[%02d:%02d:%02d.%03d]" COLOR_RESET, time_info->tm_hour, time_info->tm_min, time_info->tm_sec, milliseconds);
}

void log_msg(char const* msg) {
    log_time();
    printf(" [INFO] %s\n", msg);
}

void log_warn(char const* msg) {
    log_time();
    printf(COLOR_YELLOW " [WARN] %s" COLOR_RESET "\n", msg);
}

void log_err(char const* msg) {
    log_time();
    printf(COLOR_RED " [ERROR] %s" COLOR_RESET "\n", msg);
}