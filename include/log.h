#ifndef LOG_H
#define LOG_H

int log_init(const char *path);
void log_message(const char *level, const char *fmt, ...);
void log_close(void);

#endif
