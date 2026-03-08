#include "tools.h"

void log_msg(char const* msg) {
    struct timespec time_spec;
    timespec_get(&time_spec, TIME_UTC);
    struct tm* time_info = localtime(&time_spec.tv_sec);
    i32 milliseconds = time_spec.tv_nsec / 1000000; // 1 000 000
    printf("%02d:%02d:%02d.%03d %s\n", time_info -> tm_hour, time_info -> tm_min, time_info -> tm_sec, milliseconds, msg);
}

void log_time() {
    struct timespec time_spec;
    timespec_get(&time_spec, TIME_UTC);
    struct tm* time_info = localtime(&time_spec.tv_sec);
    i32 milliseconds = time_spec.tv_nsec / 1000000; // 1 000 000
    printf("%02d:%02d:%02d.%03d", time_info -> tm_hour, time_info -> tm_min, time_info -> tm_sec, milliseconds);
}