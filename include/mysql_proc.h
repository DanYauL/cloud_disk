#ifndef _MYSQL_PROC_H
#define _MYSQL_PROC_H

#include <mysql/mysql.h>

#define SQL_MAX_LEN         (512)   //sql语句长度

/*  
		param conn (in) 
		param msg (int) 用户错误信息
*/
void print_error(MYSQL *conn, const char *msg);
/*
		param user_name (in)    数据库用户
		param passwd 		(in)		数据库密码
		param  db_name	(in)		数据库名字
*/
MYSQL *mysql_conn(char *user_name, char *passwd, char *db_name);

//	param conn  (in)  res_set   (in)   数据库查询后的结果集
void process_result_test(MYSQL *conn, MYSQL_RES *res_set);
//处理数据库查询结果，结果集保存在buf，只处理一条记录，一个字段, 如果buf为NULL，无需保存结果集，只做判断有没有此记录
//返回值： 0成功并保存记录集，1没有记录集，2有记录集但是没有保存，-1失败
int process_result_one(MYSQL *conn, char *sql_cmd, char *buf);
#endif