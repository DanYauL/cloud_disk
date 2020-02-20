#include <stdio.h>
#include <unistd.h>
#include <zlog.h>
#include "../include/log_m.h"

void make_log(void (*log)(int, const char*), int level, const char *format)
{
    log(level, format);    
}

void mysql_log(int level, const char *format)
{
    int rc;
    rc = dzlog_init("./../conf/log_m.conf","mysql");
    if(rc){
        printf("init failed\n");
        zlog_fini();
        return;
    }
    dzlog(__FILE__,sizeof(__FILE__),__FUNCTION__,sizeof(__FUNCTION__),__LINE__,level,"mysql_log----%s",format);
    
    zlog_fini();
    return;
}
void redis_log(int level, const char *format)
{
    int rc;
    rc = dzlog_init("./../conf/log_m.conf","redis");
    if(rc){
        printf("init failed\n");
        zlog_fini();
        return;
   }
    dzlog(__FILE__,sizeof(__FILE__),__FUNCTION__,sizeof(__FUNCTION__),__LINE__,level,"redis_log-----%s",format);

    zlog_fini();
    return;
}

void upload_log(int level, const char *format)
{
    int rc;
    rc = dzlog_init("./../conf/log_m.conf","upload");
    if(rc){
        printf("init failed\n");
        zlog_fini();
        return;
    }
    dzlog(__FILE__,sizeof(__FILE__),__FUNCTION__,sizeof(__FUNCTION__),__LINE__,level,"upload_log------%s",format);

    zlog_fini();
    return;
}


