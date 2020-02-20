#ifndef _LOG_M_H
#define _LOG_M_H

typedef void (*log)(int, const char*);

void mysql_log(int level, const char *log_msg);
 
void redis_log(int level, const char *log_msg);

void upload_log(int level, const char *log_msg);

#endif
