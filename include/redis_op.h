/**
 * @file   redis_api.h
 * @brief  redis ��װ�ӿ�
 */

#ifndef _REDIS_OP_H_
#define _REDIS_OP_H_

#include <hiredis/hiredis.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "log_m.h"


#define REDIS_COMMAND_SIZE        300            /* redis Command ָ����󳤶� */
#define FIELD_ID_SIZE            100            /* redis hash��field���ֶγ��� */
#define VALUES_ID_SIZE           1024            /* redis        value���ֶγ��� */
typedef char (*RCOMMANDS)[REDIS_COMMAND_SIZE];/* redis ������� �����ַ����������� */
typedef char (*RFIELDS)[FIELD_ID_SIZE];        /* redis hash��������field�ַ����������� */

//����ָ�����ͣ������ָ�� char[1024]
typedef char (*RVALUES)[VALUES_ID_SIZE];    /* redis ��������value�ַ����������� */


/* -------------------------------------------*/
/**
 * @brief  redis tcpģʽ����
 *
 * @param ip_str	redis������ip
 * @param port_str	redis������port
 *
 * @returns   
 *			�ɹ��������Ӿ�� 
 *			ʧ�ܷ���NULL
 */
/* -------------------------------------------*/
redisContext* rop_connectdb_nopwd(char *ip_str, char* port_str);


/* -------------------------------------------*/
/**
 * @brief  redis tcpģʽ����
 *
 * @param ip_str    redis������ip
 * @param port_str  redis������port
 * @param pwd       redis����������
 *
 * @returns   
 *            �ɹ��������Ӿ�� 
 *            ʧ�ܷ���NULL
 */
/* -------------------------------------------*/
redisContext* rop_connectdb(char *ip_str, char* port_str, char *pwd);

/* -------------------------------------------*/
/**
 * @brief  redis unix��ģʽ����
 *
 * @param ip_str    unix��sock�ļ�
 * @param  pwd      redis����������
 *
 * @returns   
 *            �ɹ��������Ӿ�� 
 *            ʧ�ܷ���NULL
 */
/* -------------------------------------------*/
redisContext* rop_connectdb_unix(char *sock_path, char *pwd);

/* -------------------------------------------*/
/**
 * @brief  tcp ����redis��ʱ�ȴ�ģʽ��timeval���ӳ�ʱ
 *            ����
 *
 * @param ip_str        redis ������ip
 * @param port_str        redis �������˿�
 * @param timeval        ���ʱ�ȴ�ʱ��
 *
 * @returns   
 *        �ɹ��������Ӿ��
 *        ʧ�ܷ���NULL
 */
/* -------------------------------------------*/
redisContext* rop_connectdb_timeout(char* ip_str, char *port_str, struct timeval *timeout);


/* -------------------------------------------*/
/**
 * @brief  �ر�ָ�������Ӿ��
 *
 * @param conn    �ѽ����õ�����
 */
/* -------------------------------------------*/
void rop_disconnect(redisContext* conn);

/* -------------------------------------------*/
/**
 * @brief  ѡ��redis�� ����һ�����ݿ�
 *
 * @param conn        �����ӵ����ݿ�����
 * @param db_no        redis���ݿ���
 *
 * @returns   
 *            -1 ʧ��
 *            0  �ɹ�
 */
/* -------------------------------------------*/
int rop_selectdatabase(redisContext *conn, unsigned int db_no);

/* -------------------------------------------*/
/**
 * @brief            ��յ�ǰ���ݿ�������Ϣ(����)
 *
 * @param conn        �����ӵ����ݿ�����
 *
 * @returns   
 *            -1 ʧ��
 *            0  �ɹ�
 */
/* -------------------------------------------*/
int rop_flush_database(redisContext *conn);

/* -------------------------------------------*/
/**
 * @brief  �ж�keyֵ�Ƿ����
 *
 * @param conn        �Ѿ�����������
 * @param key        ��ҪѰ�ҵ�keyֵ
 *
 * @returns   
 *                -1 ʧ��
 *                1 ����
 *                0 ������
 */
/* -------------------------------------------*/
int rop_is_key_exist(redisContext *conn, char* key);

/* -------------------------------------------*/
/**
 * @brief            ɾ��һ��key
 *
 * @param conn        �Ѿ�����������
 * @param key        
 *
 * @returns   
 *                -1 ʧ��
 *                0 �ɹ�
 */
/* -------------------------------------------*/
int rop_del_key(redisContext *conn, char *key);


/* -------------------------------------------*/
/**
 * @brief  ��ӡ��������ƥ��pattern��key
 *
 * @param conn        �ѽ����õ�����
 * @param pattern    ƥ��ģʽ��pattern֧��glob-style��ͨ�����ʽ��
 *                    �� *��ʾ����һ�������ַ���
 *                       ?��ʾ�����ַ���
 *                    [abc]��ʾ������������һ����ĸ��
 */
/* -------------------------------------------*/
void rop_show_keys(redisContext *conn, char* pattern);

/* -------------------------------------------*/
/**
 * @brief  ����һ��key��ɾ��ʱ�� ��ϵͳ����һ��ʱ��
 *            �����Զ�ɾ����KEY
 *
 * @param conn                �Ѿ������õ�����
 * @param delete_time        �����¼� time_t ����ʱ��
 *
 * @returns   
 *        0    SUCC
 *        -1  FAIL
 */
/* -------------------------------------------*/
int rop_set_key_lifecycle(redisContext *conn, char *key, time_t delete_time);

/* -------------------------------------------*/
/**
 * @brief            �������߸���һ��HASH��
 *
 * @param conn                �ѽ����õ�����
 * @param key                hash ����
 * @param element_num        hash ���������
 * @param fields            hash ��������������char(*)[FIELD_ID_SIZE]
 * @param values            hash ������ֵ����  char(*)[VALUES_ID_SIZE]
 *
 * @returns   
 *            0   �ɹ�    
 *            -1  ʧ��
 */
/* -------------------------------------------*/
int rop_create_or_replace_hash_table(redisContext* conn,
                                     char* key, 
                                     unsigned int element_num, 
                                     RFIELDS fields, 
                                     RVALUES values);

/* -------------------------------------------*/
/**
 * @brief  ��ָ����hash�� ָ����field��Ӧ��value����num
 *
 * @param conn			�ѽ����õ�����
 * @param key			hash����
 * @param field			hash���µ�������
 *
 * @returns
 *			0		succ
 *			-1		fail
 */
/* -------------------------------------------*/
int rop_hincrement_one_field(redisContext *conn, char *key, char *field, unsigned int num);


/* -------------------------------------------*/
/**
 * @brief  ����ִ������������� ��������ͷ��
 *
 * @param conn        �ѽ����õ�����
 * @param key        ������
 * @param values    ��װ�õ�����
 * @param values    ��װ�õ�ֵ����
 * @param val_num    ֵ����
 *
 * @returns   
 *            0        succ
 *            -1        FAIL
 */
/* -------------------------------------------*/
int rop_hash_set_append(redisContext *conn, char *key, RFIELDS fields, RVALUES values, int val_num);

/* -------------------------------------------*/
/**
 * @brief  ��һ��hash�������һ�� key-value ����
 *
 * @param conn  redis����
 * @param key   ��ϣ����
 * @param field
 * @param value
 *
 * @returns
 *            0        succ
 *            -1        FAIL
 */
/* -------------------------------------------*/
int rop_hash_set(redisContext *conn, char *key, char *field, char *value);

/* -------------------------------------------*/
/**
 * @brief  ��һ��hash����ȡ��һ�� key-value ����
 *
 * @param conn  redis����
 * @param key   ��ϣ����
 * @param field �ֶ�����
 * @param value �õ������ݣ� ��Ҫ�ȿ����ڴ�
 *
 * @returns
 *            0        succ
 *            -1        FAIL
 */
/* -------------------------------------------*/
int rop_hash_get(redisContext *conn, char *key, char *field, char *value);

/* -------------------------------------------*/
/**
 * @brief  �� key ָ���Ĺ�ϣ�����Ƴ�ָ������(�ֶ�)
 *
 * @param conn  redis����
 * @param key   ��ϣ����
 * @param field �ֶ�
 *
 * @returns
 *            0        succ
 *            -1        FAIL
 */
/* -------------------------------------------*/
int rop_hash_del(redisContext *conn, char *key, char *field);


/* -------------------------------------------*/
/**
 * @brief  ����ִ������������� ��������ͷ��
 *
 * @param conn        �ѽ����õ�����
 * @param key        ������
 * @param values    ��װ�õ�ֵ����
 * @param val_num    ֵ����
 *
 * @returns   
 *            0        succ
 *            -1        FAIL
 */
/* -------------------------------------------*/
int rop_list_push_append(redisContext *conn, char *key, RVALUES values, int val_num);

/* -------------------------------------------*/
/**
 * @brief  �������ݲ�������
 *
 * @param conn        �ѽ����õ�����
 * @param key        ������
 * @param value        ����
 *
 * @returns   
 */
/* -------------------------------------------*/
int rop_list_push(redisContext *conn, char *key, char *value);

/* -------------------------------------------*/
/**
 * @brief  �õ�������Ԫ�صĸ���
 *
 * @param conn    ���Ӿ��
 * @param key    ������
 *
 * @returns   
 *            >=0 ����
 *            -1 fail
 */
/* -------------------------------------------*/
int rop_get_list_cnt(redisContext *conn, char *key);

/* -------------------------------------------*/
/**
 * @brief  ����һ����Χ�ض������е�����
 *
 * @param conn        �Ѿ�����������
 * @param key        ������
 * @param begin        �׶���ʾλ�� �� 0 ��ʼ
 * @param end        �׶ν���λ�� �� -1 ��ʼ
 *
 *                    ����ķ�Χ������� 
 *                    ����õ�ȫ����Χ(0, -1)
 *                    �������һ��Ԫ�ط�Χ(0, -2)
 *                    ǰ20�����ݷ�Χ(0, 19)
 *
 * @returns   
 *            0  SUCC
 *            -1 FAIL
 */
/* -------------------------------------------*/
int rop_trim_list(redisContext *conn, char *key, int begin, int end);

/* -------------------------------------------*/
/**
 * @brief          �õ������е�����
 *
 * @param conn		�Ѿ�����������
 * @param key		������
 *
 * @returns   
 *			0  SUCC
 *			-1 FAIL
 */
/* -------------------------------------------*/
int rop_range_list(redisContext *conn, char *key, int from_pos, int count, RVALUES values, int *get_num);


/* -------------------------------------------*/
/**
 * @brief  ����ִ���Ѿ���װ�õ�redis ����
 *
 * @param conn        �ѽ����õ�����
 * @param cmds        ��װ�õ���������
 * @param cmd_num    �������
 *
 * @returns   
 *            0        succ
 *            -1        FAIL
 */
/* -------------------------------------------*/
int rop_redis_append(redisContext *conn, RCOMMANDS cmds, int cmd_num);


/* -------------------------------------------*/
/**
 * @brief  ִ�е������� �޷���ֵ ������������
 *
 * @param conn        �ѽ���������
 * @param cmd        ��װ�õ�����
 *
 * @returns   
 *            0        succ
 *            -1        FAIL
 */
/* -------------------------------------------*/
int rop_redis_command(redisContext *conn, char *cmd);

/* -------------------------------------------*/
/**
 * @brief  ����һ��reply�Ľ������
 *            �õ���Ӧ�������ö�Ӧ�ķ�����ȡ����
 *
 * @param reply        ���ص�������
 */
/* -------------------------------------------*/
void rop_test_reply_type(redisReply *reply);


/* -------------------------------------------*/
/**
 * @brief  ����key��Ӧ��ֵΪstring���͵�value
 *            
 * @param conn          �Ѿ������õ�����
 * @param key        	keyֵ
 * @param value         valueֵ
 *
 * @returns   
 *        0    SUCC
 *        -1  FAIL
 */
/* -------------------------------------------*/
int rop_set_string(redisContext *conn, char *key, char *value);

/* -------------------------------------------*/
/**
 * @brief  ����key��Ӧ��ֵΪstring���͵�value�� ͬʱ���ù���ʱ��
 *
 * @param conn          �Ѿ������õ�����
 * @param key        	keyֵ
 * @param seconds       key����Чʱ��(����ʱ��)
 * @param value         valueֵ
 *
 * @returns
 *        0    SUCC
 *        -1  FAIL
 */
/* -------------------------------------------*/
int rop_setex_string(redisContext *conn, char *key, unsigned int seconds, char *value);

/* -------------------------------------------*/
/**
 * @brief  ��ȡkey��Ӧ��valueֵ
 *            
 * @param conn          �Ѿ������õ�����
 * @param key        	keyֵ
 * @param value         valueֵ
 *
 * @returns   
 *        0    SUCC
 *        -1  FAIL
 */
/* -------------------------------------------*/
int rop_get_string(redisContext *conn, char *key, char *value);


//==================���򼯺���ز���====================

/* -------------------------------------------*/
/**
 * @brief        ��ָ����zset������³�Ա
 *                ��key �� ��Ա������ �򴴽���
 *
 * @param conn        �ѽ���������
 * @param key         zset����
 * @param score       zset����(Ȩ��)
 * @param member      zset��Ա��
 *
 * @returns
 *            0            succ
 *            -1           fail
 */
/* -------------------------------------------*/
int rop_zset_add(redisContext *conn, char* key, long score, char* member);

/* -------------------------------------------*/
/**
 * @brief        ɾ��ָ���ĳ�Ա
 *
 *
 * @param conn       �ѽ���������
 * @param key        zset����
 * @param member     zset��Ա��
 *
 * @returns
 *            0            succ
 *            -1            fail
 */
/* -------------------------------------------*/
int rop_zset_zrem(redisContext *conn, char* key, char* member);

/* -------------------------------------------*/
/**
 * @brief        ɾ�����еĳ�Ա
 *
 *
 * @param conn       �ѽ���������
 * @param key        zset����
 *
 * @returns
 *            0            succ
 *            -1            fail
 */
/* -------------------------------------------*/
int rop_zset_del_all(redisContext *conn, char* key);

/* -------------------------------------------*/
/**
 * @brief  �����ȡ���򼯺ϵ�Ԫ��
 *          ��������Ϊ ����Ϊ
 *              [from_pos, end_pos)
 *
 * @param conn
 * @param key       ����
 * @param from_pos  ���ұ����ʼ�����±�
 * @param end_pos   ���ұ�Ľ�β�����±�
 * @param values    �õ����е�value����
 *      RVALUESΪ����ָ�����ͣ������ָ�� char[1024]
 *      typedef char (*RVALUES)[VALUES_ID_SIZE];
 * @param get_num   �õ����value�ĸ���
 *
 * @returns
 *      0 succ, -1 fail
 */
/* -------------------------------------------*/
extern int rop_zset_zrevrange(redisContext *conn, char *key, int from_pos, int end_pos, RVALUES values, int *get_num);


/* -------------------------------------------*/
/**
 * @brief        ��ָ����zset����Ӧ�ĳ�Ա��ֵ����1
 *                ��key �� ��Ա������ �򴴽���
 *
 * @param conn        �ѽ���������
 * @param key        zset����
 * @param member     zset��Ա��
 *
 * @returns
 *            0            succ
 *            -1            fail
 */
/* -------------------------------------------*/
int rop_zset_increment(redisContext *conn, char* key, char* member);

/* -------------------------------------------*/
/**
 * @brief  �õ�������Ԫ�صĸ���
 *
 * @param conn	���Ӿ��
 * @param key	������
 *
 * @returns
 *			>=0 ����
 *			-1 fail
 */
/* -------------------------------------------*/
int rop_zset_zcard(redisContext *conn, char *key);


/* -------------------------------------------*/
/**
 * @brief        �õ�zsetһ��member��score
 *
 * @param conn        �ѽ���������
 * @param key        zset����
 * @param member    zset��Ա��
 *
 * @returns
 *             >=0            succ
 *            -1            fail
 */
/* -------------------------------------------*/
int rop_zset_get_score(redisContext *conn, char *key, char *member);

/* -------------------------------------------*/
/**
 * @brief        �ж�ĳ����Ա�Ƿ����
 *
 * @param conn      �ѽ���������
 * @param key       zset����
 * @param member    zset��Ա��
 *
 * @returns
 *            0            ������
 *            1            ����
 *            -1           ����
 */
/* -------------------------------------------*/
extern int rop_zset_exit(redisContext *conn, char *key, char *member);


/* -------------------------------------------*/
/**
 * @brief     ������ָ����zset����Ӧ�ĳ�Ա��ֵ����1
 *                ��key �� ��Ա������ �򴴽���
 *
 * @param conn        �ѽ����õ�����
 * @param key         ���򼯺�����
 * @param values      ��װ�õĳ�Ա����
 * @param val_num     ���ݸ���
 *
 * @returns
 *            0        succ
 *            -1        FAIL
 */
/* -------------------------------------------*/
int rop_zset_increment_append(redisContext *conn, char *key, RVALUES values, int val_num);

#endif
