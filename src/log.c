#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include "log.h"

static FILE *log_file = NULL;
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

int log_init(const char *path)
{
    pthread_mutex_lock(&log_mutex);
    if (log_file != NULL)
    {
        fclose(log_file);
    }
    log_file = fopen(path, "a");
    pthread_mutex_unlock(&log_mutex);

    if (log_file == NULL)
    {
        perror("fopen log");
        return -1;
    }
    return 0;
}

void log_message(const char *level, const char *fmt, ...)
{
    pthread_mutex_lock(&log_mutex);

    if (log_file == NULL)
    {
        pthread_mutex_unlock(&log_mutex);
        return;
    }

    time_t now = time(NULL);
    struct tm tm_buf;
    localtime_r(&now, &tm_buf);

    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &tm_buf);

    fprintf(log_file, "[%s] [%s] ", timestamp, level);

    va_list args;
    va_start(args, fmt);
    vfprintf(log_file, fmt, args);
    va_end(args);

    fputc('\n', log_file);
    fflush(log_file);

    pthread_mutex_unlock(&log_mutex);
}

void log_close(void)
{
    pthread_mutex_lock(&log_mutex);
    if (log_file != NULL)
    {
        fclose(log_file);
        log_file = NULL;
    }
    pthread_mutex_unlock(&log_mutex);
}
