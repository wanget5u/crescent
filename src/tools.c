#include <tools.h>
#include <stdio.h>
#include <time.h>

void print_hour() {
    time_t raw_time;
    time(&raw_time);
    struct tm* time_info = localtime(&raw_time);
    printf("%02d:%02d:%02d", time_info -> tm_hour, time_info -> tm_min, time_info -> tm_sec);
}