#include "../include/util_cgi.h"
#include "../include/log_m.h"
void test()
{
    make_log(mysql_log,ZLOG_LEVEL_ERROR,"wrong num ");
    make_log(mysql_log,ZLOG_LEVEL_INFO,"USER INFO");
}
int main()
{
    test();
}
