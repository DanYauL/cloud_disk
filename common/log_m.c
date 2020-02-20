#include <stdio.h>
#include <unistd.h>
#include <zlog.h>
#include "../include/log_m.h"

void mysql_log(int level, const char *log_msg)
{
    int rc;
    rc = dzlog_init("./../conf/log_m.conf","mysql");
    if(rc == 0){
        printf("init failed\n");
        zlog_fini();
        return;
    }
    dzlog(__FILE__,sizeof(__FILE__),__FUNCTION__,sizeof(__FUNCTION__),__LINE__,level,log_msg);

    zlog_fini();
    return;
}
void redis_log(int level, const char *log_msg)
{
    int rc;
    rc = dzlog_init("./../conf/log_m.conf","redis");
    if(rc == 0){
        printf("init failed\n");
        zlog_fini();
        return;
   }
    dzlog(__FILE__,sizeof(__FILE__),__FUNCTION__,sizeof(__FUNCTION__),__LINE__,level,log_msg);

    zlog_fini();
    return;
}
void upload_log(int level, const char *log_msg)
{
    int rc;
    rc = dzlog_init("./../conf/log_m.conf","upload");
    if(rc == 0){
        printf("init failed\n");
        zlog_fini();
        return;
    }
    dzlog(__FILE__,sizeof(__FILE__),__FUNCTION__,sizeof(__FUNCTION__),__LINE__,level,log_msg);

    zlog_fini();
    return;
}

