#ifndef _MYSQL_PROC_H
#define _MYSQL_PROC_H

#include <mysql/mysql.h>

#define SQL_MAX_LEN         (512)   //sql��䳤��

/*  
		param conn (in) 
		param msg (int) �û�������Ϣ
*/
void print_error(MYSQL *conn, const char *msg);
/*
		param user_name (in)    ���ݿ��û�
		param passwd 		(in)		���ݿ�����
		param  db_name	(in)		���ݿ�����
*/
MYSQL *mysql_conn(char *user_name, char *passwd, char *db_name);

//	param conn  (in)  res_set   (in)   ���ݿ��ѯ��Ľ����
void process_result_test(MYSQL *conn, MYSQL_RES *res_set);
//�������ݿ��ѯ����������������buf��ֻ����һ����¼��һ���ֶ�, ���bufΪNULL�����豣��������ֻ���ж���û�д˼�¼
//����ֵ�� 0�ɹ��������¼����1û�м�¼����2�м�¼������û�б��棬-1ʧ��
int process_result_one(MYSQL *conn, char *sql_cmd, char *buf);
#endif