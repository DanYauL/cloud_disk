#ifndef _LOG_M_H
#define _LOG_M_H

void make_log(void(*log)(int,const char *),int level, const char *format);

void mysql_log(int level, const char *format);
 
void redis_log(int level, const char *format);

void upload_log(int level, const char *format);

#endif
