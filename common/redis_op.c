/**
 * @file redis_op.c
 * @brief  redis ���������ӿں�key�Ĳ���ʵ��
 */

#include "../include/redis_op.h"
#include "../include/log_m.h"
#include <zlog.h>   //log ��
/* -------------------------------------------*/
/**
 * @brief  ѡ��redisһ�����ݿ�
 *
 * @param conn		�����ӵ����ݿ�����
 * @param db_no		redis���ݿ���
 *
 * @returns   
 *			-1 ʧ��
 *			0  �ɹ�
 */
/* -------------------------------------------*/
int rop_selectdatabase(redisContext *conn, unsigned int db_no)
{
    int retn = 0;
    redisReply *reply = NULL;

    /* ѡ��һ�����ݿ� */
    reply = (redisReply*)redisCommand(conn, "select %d", db_no);
    if (reply == NULL) {
        fprintf(stderr, "[-][GMS_REDIS]Select database %d error!\n", db_no);
        make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS]Select databas error!");
        retn = -1;
        goto END;
    }

    printf("[+][GMS_REDIS]Select database %d SUCCESS!\n", db_no);
    make_log(redis_log, ZLOG_LEVEL_INFO, "[+][GMS_REDIS]Select database success");
END:
    freeReplyObject(reply);
    return retn;
}


/* -------------------------------------------*/
/**
 * @brief			��յ�ǰ���ݿ�������Ϣ(����)
 *
 * @param conn		�����ӵ����ݿ�����
 *
 * @returns   
 *			-1 ʧ��
 *			0  �ɹ�
 */
/* -------------------------------------------*/
int rop_flush_database(redisContext *conn)
{
    int retn = 0;	
    redisReply *reply = NULL;

    reply = (redisReply *)redisCommand(conn, "FLUSHDB");
    if (reply == NULL) {
        fprintf(stderr, "[-][GMS_REDIS]Clear all data error\n");
        make_log(redis_log, ZLOG_LEVEL_ERROR, "[+][GMS_REDIS]clear all data error!!\n");

        retn = -1;
        goto END;
    }

    printf("[+][GMS_REDIS]Clear all data!!\n");
    make_log(redis_log, ZLOG_LEVEL_INFO, "[+][GMS_REDIS]clear all data!!\n");

END:
    freeReplyObject(reply);
    return retn;
}

/* -------------------------------------------*/
/**
 * @brief  �ж�keyֵ�Ƿ����
 *
 * @param conn		�Ѿ�����������
 * @param key		��ҪѰ�ҵ�keyֵ
 *
 * @returns   
 *				-1 ʧ��
 *				1 ����
 *				0 ������
 */
/* -------------------------------------------*/
int rop_is_key_exist(redisContext *conn, char* key)
{
    int retn = 0;	

    redisReply *reply = NULL;

    reply = (redisReply*)redisCommand(conn, "EXISTS %s", key);
    //rop_test_reply_type(reply);
    if (reply->type != REDIS_REPLY_INTEGER) {
        fprintf(stderr, "[-][GMS_REDIS]is key exist get wrong type!\n");     
        make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS]is key exist get wrong type!\n");
        retn = -1;
        goto END;
    }

    if (reply->integer == 1) {
        retn = 1;	
    }
    else {
        retn = 0;
    }

END:
    freeReplyObject(reply);
    return retn;
}

/* -------------------------------------------*/
/**
 * @brief			ɾ��һ��key
 *
 * @param conn		�Ѿ�����������
 * @param key		
 *
 * @returns   
 *				-1 ʧ��
 *				0 �ɹ�
 */
/* -------------------------------------------*/
int rop_del_key(redisContext *conn, char *key)
{
    int retn = 0;
    redisReply *reply = NULL;

    reply = (redisReply*)redisCommand(conn, "DEL %s", key);
    if (reply->type != REDIS_REPLY_INTEGER) {
        fprintf(stderr, "[-][GMS_REDIS] DEL key %s ERROR\n", key);
        make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS] DEL key %s ERROR\n");
        retn = -1;
        goto END;
    }

    if (reply->integer > 0) {
        retn = 0;	
    }
    else {
        retn = -1;
    }

END:
    freeReplyObject(reply);
    return retn;
}

/* -------------------------------------------*/
/**
 * @brief  ����һ��key��ɾ��ʱ�� ��ϵͳ����һ��ʱ��
 *			�����Զ�ɾ����KEY
 *
 * @param conn				�Ѿ������õ�����
 * @param delete_time		�����¼� time_t ����ʱ��
 *
 * @returns   
 *		0	SUCC
 *		-1  FAIL
 */
/* -------------------------------------------*/
int rop_set_key_lifecycle(redisContext *conn, char *key, time_t delete_time)
{
    int retn = 0;
    redisReply *reply = NULL;		

    reply = (redisReply*)redisCommand(conn, "EXPIREAT %s %d", key, delete_time);
    if (reply->type != REDIS_REPLY_INTEGER) {
        fprintf(stderr, "[-][GMS_REDIS]Set key:%s delete time ERROR!\n", key);
        make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS]Set key delete time ERROR! \n");
        retn = -1;
    }
    if (reply->integer == 1) {
        /* �ɹ� */
        retn = 0;
    }
    else {
        /* ���� */
        retn = -1;
    }


    freeReplyObject(reply);	
    return retn;
}

/* -------------------------------------------*/
/**
 * @brief  ��ӡ��������ƥ��pattern��key
 *
 * @param conn		�ѽ����õ�����
 * @param pattern	ƥ��ģʽ��pattern֧��glob-style��ͨ�����ʽ��
 *					�� *��ʾ����һ�������ַ���
 *					   ?��ʾ�����ַ���
 *				    [abc]��ʾ������������һ����ĸ��
 */
/* -------------------------------------------*/
void rop_show_keys(redisContext *conn, char* pattern)
{
    int i = 0;
    redisReply *reply = NULL;

    reply = (redisReply*)redisCommand(conn, "keys %s", pattern);
    if (reply->type != REDIS_REPLY_ARRAY) {
        fprintf(stderr, "[-][GMS_REDIS]show all keys and data wrong type!\n");
        make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS]show all keys and data wrong type!\n");
        goto END;
    }

    for (i = 0; i < reply->elements; ++i) {
        printf("======[%s]======\n", reply->element[i]->str);
    }


END:
    freeReplyObject(reply);
}

/* -------------------------------------------*/
/**
 * @brief  ����ִ���Ѿ���װ�õ�redis ����
 *
 * @param conn		�ѽ����õ�����
 * @param cmds		��װ�õ���������
 * @param cmd_num	�������
 *
 * @returns   
 *			0		succ
 *			-1		FAIL
 */
/* -------------------------------------------*/
int rop_redis_append(redisContext *conn, RCOMMANDS cmds, int cmd_num)
{
    int retn = 0;
    int i = 0;
    redisReply *reply = NULL;


    /* �������������������ܵ� */
    for (i = 0; i < cmd_num; ++i) {
        retn = redisAppendCommand(conn, cmds[i]);
        if (retn != REDIS_OK) {
            fprintf(stderr, "[-][GMS_REDIS]Append Command: %s ERROR!\n", cmds[i]);
           	make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS]Append Command ERROR!!\n");
            retn = -1;
            goto END;
        }
        retn = 0;
    }

    /* �ύ���� */
    for (i = 0; i < cmd_num; ++i) {
        retn = redisGetReply(conn, (void**)&reply);
        if (retn != REDIS_OK) {
            retn = -1;
            fprintf(stderr, "[-][GMS_REDIS]Commit Command:%s ERROR!\n", cmds[i]);
            make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS]Commit Command ERROR!!\n");
            freeReplyObject(reply);
            break;
        }
        freeReplyObject(reply);
        retn = 0;
    }

END:
    return retn;
}

/* -------------------------------------------*/
/**
 * @brief  ִ�е������� �޷���ֵ ������������
 *
 * @param conn		�ѽ���������
 * @param cmd		��װ�õ�����
 *
 * @returns   
 *			0		succ
 *			-1		FAIL
 */
/* -------------------------------------------*/
int rop_redis_command(redisContext *conn, char *cmd)
{
    int retn = 0;

    redisReply *reply = NULL;

    reply = (redisReply*)redisCommand(conn, cmd);
    if (reply == NULL) {
        make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS]Commit Command ERROR!!\n");
        retn = -1;
    }

    freeReplyObject(reply);

    return retn;
}

/* -------------------------------------------*/
/**
 * @brief  ����һ��reply�Ľ������
 *			�õ���Ӧ�������ö�Ӧ�ķ�����ȡ����
 *
 * @param reply		���ص�������
 */
/* -------------------------------------------*/
void rop_test_reply_type(redisReply *reply)
{
    switch (reply->type) {
    case REDIS_REPLY_STATUS:
        make_log(redis_log, ZLOG_LEVEL_DEBUG, "[+][GMS_REDIS]=REDIS_REPLY_STATUS=[string] use reply->str to get data, reply->len get data len\n");
        break;
    case REDIS_REPLY_ERROR:
        make_log(redis_log, ZLOG_LEVEL_DEBUG, "[+][GMS_REDIS]=REDIS_REPLY_ERROR=[string] use reply->str to get data, reply->len get date len\n"); 
        break;
    case REDIS_REPLY_INTEGER:
    		make_log(redis_log, ZLOG_LEVEL_DEBUG, "[+][GMS_REDIS]=REDIS_REPLY_INTEGER=[long long] use reply->integer to get data\n");
        break;
    case REDIS_REPLY_NIL:
    		make_log(redis_log, ZLOG_LEVEL_DEBUG, "[+][GMS_REDIS]=REDIS_REPLY_NIL=[] data not exist\n");  
        break;
    case REDIS_REPLY_ARRAY:
        make_log(redis_log, ZLOG_LEVEL_DEBUG, "[+][GMS_REDIS]=REDIS_REPLY_ARRAY=[array] use reply->elements to get number of data, reply->element[index] to get (struct redisReply*) Object\n");
        break;
    case REDIS_REPLY_STRING:
        make_log(redis_log, ZLOG_LEVEL_DEBUG, "[+][GMS_REDIS]=REDIS_REPLY_string=[string] use reply->str to get data, reply->len get data len\n");
        break;
    default:
        make_log(redis_log, ZLOG_LEVEL_DEBUG, "[-][GMS_REDIS]Can't parse this type\n");
        break;
    }
}


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
redisContext* rop_connectdb_nopwd(char *ip_str, char* port_str)
{
    redisContext *conn = NULL;
    uint16_t port = atoi(port_str);

    conn = redisConnect(ip_str, port);

    if (conn  == NULL) {
    		make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS]Connect Error:Can't allocate redis context!\n");		
        goto END;
    }

    if (conn->err) {	
        make_log(redis_log, ZLOG_LEVEL_ERROR,	"[-][GMS_REDIS]Connect Error\n");
        redisFree(conn);
        conn = NULL;
        goto END;
    }
    make_log(redis_log, ZLOG_LEVEL_INFO, "[+][GMS_REDIS]Connect SUCCESS!\n");

END:
    return conn;
}


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
redisContext* rop_connectdb(char *ip_str, char* port_str, char *pwd)
{
    redisContext *conn = NULL;
    uint16_t port = atoi(port_str);
    char auth_cmd[REDIS_COMMAND_SIZE];

    conn = redisConnect(ip_str, port);

    if (conn  == NULL) {	
        make_log(redis_log, ZLOG_LEVEL_ERROR,	"[-][GMS_REDIS]Connect Error:Can't allocate redis context!\n");
        goto END;
    }

    if (conn->err) {
        make_log(redis_log, ZLOG_LEVEL_ERROR,	"[-][GMS_REDIS]Connect Error\n");
        redisFree(conn);
        conn = NULL;
        goto END;
    }

    redisReply *reply = NULL;
    sprintf(auth_cmd, "auth %s", pwd);

    reply = redisCommand(conn, auth_cmd);
    if (reply == NULL) {
        make_log(redis_log, ZLOG_LEVEL_ERROR,	"[-][GMS_REDIS]Connect : auth Error\n");
        conn = NULL;
        goto END;
    }
    freeReplyObject(reply);
    make_log(redis_log, ZLOG_LEVEL_INFO,	"[+][GMS_REDIS]Connect %s:%d SUCCESS!\n");

END:
    return conn;
}

/* -------------------------------------------*
 **
 * @brief  redis unix��ģʽ����
 *
 * @param ip_str	unix��sock�ļ�
 *
 * @returns   
 *			�ɹ��������Ӿ�� 
 *			ʧ�ܷ���NULL
 */
/* -------------------------------------------*/
redisContext* rop_connectdb_unix(char *sock_path, char *pwd)
{
    redisContext *conn = NULL;
    char auth_cmd[REDIS_COMMAND_SIZE];

    conn = redisConnectUnix(sock_path);
    if (conn  == NULL) {
       make_log(redis_log, ZLOG_LEVEL_INFO, "[-][GMS_REDIS]Connect domain-unix:%s Error:Can't allocate redis context!\n", sock_path);		
        goto END;
    }

    if (conn->err) {
       make_log(redis_log, ZLOG_LEVEL_INFO, "[-][GMS_REDIS]Connect domain-unix:%s Error:%s\n", sock_path, conn->errstr);	
        redisFree(conn);
        conn = NULL;
        goto END;
    }

    redisReply *reply = NULL;
    sprintf(auth_cmd, "auth %s", pwd);
    reply = redisCommand(conn, auth_cmd);
    if (reply == NULL) {
       make_log(redis_log, ZLOG_LEVEL_INFO, "[-][GMS_REDIS]Command : auth %s ERROR!\n", pwd);
        conn = NULL;
        goto END;
    }
    freeReplyObject(reply);

   make_log(redis_log, ZLOG_LEVEL_INFO,"[+][GMS_REDIS]Connect domain-unix:%s SUCCESS!\n", sock_path);

END:
    return conn;
}

/* -------------------------------------------*/
/**
 * @brief  tcp ����redis��ʱ�ȴ�ģʽ��timeval���ӳ�ʱ
 *			����
 *
 * @param ip_str		redis ������ip
 * @param port_str		redis �������˿�
 * @param timeval		���ʱ�ȴ�ʱ��
 *
 * @returns   
 *		�ɹ��������Ӿ��
 *		ʧ�ܷ���NULL
 */
/* -------------------------------------------*/
redisContext* rop_connectdb_timeout(char* ip_str, char *port_str, struct timeval *timeout)
{
    redisContext *conn = NULL;
    uint16_t port = atoi(port_str);


    conn = redisConnectWithTimeout(ip_str, port, *timeout);

    if (conn  == NULL) {
       make_log(redis_log, ZLOG_LEVEL_INFO, "[-][GMS_REDIS]Connect %s:%d Error:Can't allocate redis context!\n", ip_str, port);
        goto END;
    }

    if (conn->err) {
       make_log(redis_log, ZLOG_LEVEL_INFO, "[-][GMS_REDIS]Connect %s:%d Error:%s\n", ip_str, port, conn->errstr);	
        redisFree(conn);
        conn = NULL;
        goto END;
    }

   make_log(redis_log, ZLOG_LEVEL_INFO,"[+][GMS_REDIS]Connect %s:%d SUCCESS!\n", ip_str, port);

END:
    return conn;
}

/* -------------------------------------------*/
/**
 * @brief  �ر�ָ�������Ӿ��
 *
 * @param conn	�ѽ����õ�����
 */
/* -------------------------------------------*/
void rop_disconnect(redisContext* conn)
{
    if (conn == NULL) {
        return ;
    }
    redisFree(conn);

   make_log(redis_log, ZLOG_LEVEL_INFO,"[+][GMS_REDIS]Disconnect SUCCESS!\n");
}

/* ��װһ�� hmset ���� */
static char* make_hmset_command(char* key, unsigned int element_num, RFIELDS fields, RVALUES values)
{
    char *cmd = NULL;	
    unsigned int buf_size = 0;
    unsigned int use_size = 0;
    unsigned int i = 0;

    cmd = (char*)malloc(1024*1024);
    if (cmd == NULL) {
        goto END;
    }
    memset(cmd, 0, 1024*1024);
    buf_size += 1024*1024;

    strncat(cmd, "hmset", 6);
    use_size += 5;
    strncat(cmd, " ", 1);
    use_size += 1;

    strncat(cmd, key, 200);
    use_size += 200;

    for (i = 0; i < element_num; ++i) {

        strncat(cmd, " ", 1);
        use_size += 1;
        if (use_size >= buf_size) {
            cmd = realloc(cmd, use_size + 1024*1024);
            if (cmd == NULL) {
                goto END;
            }
            buf_size += 1024*1024;
        }

        strncat(cmd, fields[i], FIELD_ID_SIZE);
        use_size += strlen(fields[i]);
        if (use_size >= buf_size) {
            cmd = realloc(cmd, use_size + 1024*1024);
            if (cmd == NULL) {
                goto END;
            }
            buf_size += 1024*1024;
        }


        strncat(cmd, " ", 1);
        use_size += 1;
        if (use_size >= buf_size) {
            cmd = realloc(cmd, use_size + 1024*1024);
            if (cmd == NULL) {
                goto END;
            }
            buf_size += 1024*1024;
        }

        strncat(cmd, values[i], VALUES_ID_SIZE);
        use_size += strlen(values[i]);
        if (use_size >= buf_size) {
            cmd = realloc(cmd, use_size + 1024*1024);
            if (cmd == NULL) {
                goto END;
            }
            buf_size += 1024*1024;
        }

    }

END:
    return cmd;
}


/* -------------------------------------------*/
/**
 * @brief  ����ִ������������� ��������ͷ��
 *
 * @param conn		�ѽ����õ�����
 * @param key		������
 * @param values	��װ�õ�ֵ����
 * @param val_num	ֵ����
 *
 * @returns   
 *			0		succ
 *			-1		FAIL
 */
/* -------------------------------------------*/
int rop_hash_set_append(redisContext *conn, char *key, RFIELDS fields, RVALUES values, int val_num)
{
    int retn = 0;
    int i = 0;
    redisReply *reply = NULL;

    /* �������������������ܵ� */
    for (i = 0; i < val_num; ++i) {
        retn = redisAppendCommand(conn, "hset %s %s %s", key, fields[i], values[i]);
        if (retn != REDIS_OK) {
           make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS]HSET ERROR!\n");
            retn = -1;
            goto END;
        }
        retn = 0;
    }

    /* �ύ���� */
    for (i = 0; i < val_num; ++i) {
        retn = redisGetReply(conn, (void**)&reply);
        if (retn != REDIS_OK) {
            retn = -1;
           make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS]Commit HSET ERROR!\n");
            freeReplyObject(reply);
            break;
        }
        freeReplyObject(reply);
        retn = 0;
    }

END:
    return retn;
}

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
int rop_hash_set(redisContext *conn, char *key, char *field, char *value)
{
    int retn = 0;
    redisReply *reply = NULL;

    reply =  redisCommand(conn, "hset %s %s %s", key, field, value);
    if (reply == NULL || reply->type != REDIS_REPLY_INTEGER) {
       make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS]hset error\n");
        retn =  -1;
        goto END;
    }


END:
    freeReplyObject(reply);

    return retn;
}

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
int rop_hash_get(redisContext *conn, char *key, char *field, char *value)
{
    int retn = 0;
    int len = 0;

    redisReply *reply = NULL;

    reply =  redisCommand(conn, "hget %s %s", key, field);
    if (reply == NULL || reply->type != REDIS_REPLY_STRING) {
       make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS]hget error\n");
        retn =  -1;
        goto END;
    }


    len = reply->len > VALUES_ID_SIZE? VALUES_ID_SIZE:reply->len ;

    strncpy(value, reply->str, len);

    value[len] = '\0';


END:
    freeReplyObject(reply);


    return retn;
}

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
int rop_hash_del(redisContext *conn, char *key, char *field)
{
    int retn = 0;
    redisReply *reply = NULL;

    reply =  redisCommand(conn, "hdel %s %s", key, field);
    if (reply->integer != 1)
    {
       make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS]hdel error\n");
        retn =  -1;
        goto END;
    }


END:
    freeReplyObject(reply);

    return retn;

}


/* -------------------------------------------*/
/**
 * @brief			�������߸���һ��HASH��
 *
 * @param conn				�ѽ����õ�����
 * @param key				hash ����
 * @param element_num		hash ���������
 * @param fields			hash ��������������char(*)[FIELD_ID_SIZE]
 * @param values			hash ������ֵ����  char(*)[VALUES_ID_SIZE]
 *
 * @returns   
 *			0   �ɹ�	
 *			-1  ʧ��
 */
/* -------------------------------------------*/
int rop_create_or_replace_hash_table(redisContext* conn,
                                     char* key, 
                                     unsigned int element_num, 
                                     RFIELDS fields, 
                                     RVALUES values)
{
    int retn = 0;
    redisReply *reply = NULL;			

    char *cmd = make_hmset_command(key, element_num, fields, values);		
    if (cmd == NULL) {
       make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS]create hash table error\n");
        retn = -1;
        goto END_WITHOUT_FREE;
    }

    reply = redisCommand(conn, cmd);
    //	rop_test_reply_type(reply);
    if (strcmp(reply->str, "OK") != 0) {
       make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS]Create hash table Error\n");

        retn = -1;
        goto END;
    }


END:
    free(cmd);
    freeReplyObject(reply);

END_WITHOUT_FREE:

    return retn;
}

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
int rop_hincrement_one_field(redisContext *conn, char *key, char *field, unsigned int num)
{
    int retn = 0;

    redisReply *reply = NULL;

    reply = redisCommand(conn, "HINCRBY %s %s %d", key, field, num);
    if (reply == NULL) {
       make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS]increment error\n");	
        retn =  -1;
        goto END;
    }

END:
    freeReplyObject(reply);

    return retn;
}


/* -------------------------------------------*/
/**
 * @brief  ����ִ������������� ��������ͷ��
 *
 * @param conn		�ѽ����õ�����
 * @param key		������
 * @param values	��װ�õ�ֵ����
 * @param val_num	ֵ����
 *
 * @returns   
 *			0		succ
 *			-1		FAIL
 */
/* -------------------------------------------*/
int rop_list_push_append(redisContext *conn, char *key, RVALUES values, int val_num)
{
    int retn = 0;
    int i = 0;
    redisReply *reply = NULL;


    /* �������������������ܵ� */
    for (i = 0; i < val_num; ++i) {
        retn = redisAppendCommand(conn, "lpush %s %s", key, values[i]);
        if (retn != REDIS_OK) {
           make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS]PLUSH ERROR!\n");
            retn = -1;
            goto END;
        }
        retn = 0;
    }

    /* �ύ���� */
    for (i = 0; i < val_num; ++i) {
        retn = redisGetReply(conn, (void**)&reply);
        if (retn != REDIS_OK) {
            retn = -1;
           make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS]Commit LPUSH ERROR!\n");
            freeReplyObject(reply);
            break;
        }
        freeReplyObject(reply);
        retn = 0;
    }

END:
    return retn;
}

/* -------------------------------------------*/
/**
 * @brief  �������ݲ�������
 *
 * @param conn		�ѽ����õ�����
 * @param key		������
 * @param value		����
 *
 * @returns   
 */
/* -------------------------------------------*/
int rop_list_push(redisContext *conn, char *key, char *value)
{
    int retn = 0;
    redisReply *reply = NULL;

    reply = redisCommand(conn, "LPUSH %s %s", key, value);
    //rop_test_reply_type(reply);	
    if (reply->type != REDIS_REPLY_INTEGER) {
       make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS]LPUSH error!\n");
        retn = -1;
    }

    freeReplyObject(reply);
    return retn;
}

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
int rop_get_list_cnt(redisContext *conn, char *key)
{
    int cnt = 0;

    redisReply *reply = NULL;

    reply = redisCommand(conn, "LLEN %s", key);
    if (reply->type != REDIS_REPLY_INTEGER) {
       make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS]LLEN %s error %s\n");
        cnt = -1;
        goto END;
    }

    cnt = reply->integer;

END:
    freeReplyObject(reply);
    return cnt;
}


/* -------------------------------------------*/
/**
 * @brief  ����һ����Χ�ض������е�����
 *
 * @param conn		�Ѿ�����������
 * @param key		������
 * @param begin		�׶���ʾλ�� �� 0 ��ʼ
 * @param end		�׶ν���λ�� �� -1 ��ʼ
 *
 *					����ķ�Χ������� 
 *					����õ�ȫ����Χ(0, -1)
 *					�������һ��Ԫ�ط�Χ(0, -2)
 *					ǰ20�����ݷ�Χ(0, 19)
 *
 * @returns   
 *			0  SUCC
 *			-1 FAIL
 */
/* -------------------------------------------*/
int rop_trim_list(redisContext *conn, char *key, int begin, int end)
{
    int retn = 0;
    redisReply *reply = NULL;

    reply = redisCommand(conn, "LTRIM %s %d %d", key, begin, end);
    if (reply->type != REDIS_REPLY_STATUS) {
       make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS]LTRIM error!\n");
        retn = -1;
    }

    freeReplyObject(reply);
    return retn;
}


/* -------------------------------------------*/
/**
 * @brief  rop_range_list �õ������е�����
 *
 *          ��������Ϊ ����Ϊ
 *              [from_pos, end_pos)
 *
 * @param conn
 * @param key       ����
 * @param from_pos  ���ұ����ʼ�����±�
 * @param end_pos   ���ұ�Ľ�β�����±�
 * @param values    �õ����е�value����
 * @param get_num   �õ����value�ĸ���
 *
 * @returns   
 *      0 succ, -1 fail
 */
/* -------------------------------------------*/
int rop_range_list(redisContext *conn, char *key, int from_pos, int end_pos, RVALUES values, int *get_num)
{
    int retn = 0;
    int i = 0;
    redisReply *reply = NULL;
    int max_count = 0;

    int count = end_pos - from_pos + 1;

    reply = redisCommand(conn, "LRANGE %s %d %d", key, from_pos, end_pos);
    if (reply->type != REDIS_REPLY_ARRAY || reply->elements == 0) {
       make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS]LRANGE  error!\n");
        retn = -1;
        goto END;
    }


    max_count = (reply->elements > count) ? count: reply->elements;
    *get_num = max_count;


    for (i = 0; i < max_count; ++i) {
        strncpy(values[i], reply->element[i]->str, VALUES_ID_SIZE-1);
    }

END:
    if(reply != NULL)
    {
        freeReplyObject(reply);
    }

    return retn;
}

int rop_set_string(redisContext *conn, char *key, char *value)
{
    int retn = 0;
    redisReply *reply = NULL;
    reply = redisCommand(conn, "set %s %s", key, value);
    //rop_test_reply_type(reply);
    if (strcmp(reply->str, "OK") != 0) {
        retn = -1;
        goto END;
    }

    //printf("%s\n", reply->str);

END:

    freeReplyObject(reply);
    return retn;
}

int rop_setex_string(redisContext *conn, char *key, unsigned int seconds, char *value)
{
    int retn = 0;
    redisReply *reply = NULL;
    reply = redisCommand(conn, "setex %s %u %s", key, seconds, value);
    //rop_test_reply_type(reply);
    if (strcmp(reply->str, "OK") != 0) {
        retn = -1;
        goto END;
    }

    //printf("%s\n", reply->str);

END:

    freeReplyObject(reply);
    return retn;

}

int rop_get_string(redisContext *conn, char *key, char *value)
{
    int retn = 0;
    redisReply *reply = NULL;
    reply = redisCommand(conn, "get %s", key);
    //rop_test_reply_type(reply);
    if (reply->type != REDIS_REPLY_STRING) {
        retn = -1;
        goto END;
    }

    strncpy(value, reply->str, reply->len);
    value[reply->len] = '\0'; //�ַ���������

END:

    freeReplyObject(reply);
    return retn;
}

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
int rop_zset_add(redisContext *conn, char* key, long score, char* member)
{
    int retn = 0;
    redisReply *reply = NULL;

    //ִ������, reply->integer�ɹ�����1��reply->integerʧ�ܷ���0
    reply = redisCommand(conn, "ZADD %s %ld %s", key, score, member);
    //rop_test_reply_type(reply);

    if (reply->integer != 1)
    {
       make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS]ZADD:member Error\n");
        retn = -1;
        goto END;
    }

END:

    freeReplyObject(reply);
    return retn;

}

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
int rop_zset_zrem(redisContext *conn, char* key, char* member)
{
    int retn = 0;
    redisReply *reply = NULL;

    //ִ������, reply->integer�ɹ�����1��reply->integerʧ�ܷ���0
    reply = redisCommand(conn, "ZREM %s %s", key, member);
    //rop_test_reply_type(reply);

    if (reply->integer != 1)
    {
       make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS]ZREM:member Error\n");
        retn = -1;
        goto END;
    }

END:

    freeReplyObject(reply);
    return retn;

}

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
int rop_zset_del_all(redisContext *conn, char* key)
{
    int retn = 0;
    redisReply *reply = NULL;

    //ִ������
    reply = redisCommand(conn, "ZREMRANGEBYRANK %s 0 -1", key);
    //rop_test_reply_type(reply);

    if (reply->type != REDIS_REPLY_INTEGER) //�����������
    {
       make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS]ZREMRANGEBYRANK Error\n");
        retn = -1;
        goto END;
    }

END:

    freeReplyObject(reply);
    return retn;

}

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
int rop_zset_zrevrange(redisContext *conn, char *key, int from_pos, int end_pos, RVALUES values, int *get_num)
{
    int retn = 0;
    int i = 0;
    redisReply *reply = NULL;
    int max_count = 0;

    int count = end_pos - from_pos + 1; //����Ԫ�ظ���

    //�����ȡ���򼯺ϵ�Ԫ��
    reply = redisCommand(conn, "ZREVRANGE %s %d %d", key, from_pos, end_pos);
    if (reply->type != REDIS_REPLY_ARRAY) //������ز�������
    {
       make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS]ZREVRANGE error!\n");
        retn = -1;
        goto END;
    }

    //����һ�����飬�鿴elements��ֵ(�������)
    //ͨ��element[index] �ķ�ʽ��������Ԫ��
    //ÿ������Ԫ����һ��redisReply�����ָ��

    max_count = (reply->elements > count) ? count: reply->elements;
    *get_num = max_count; //�õ����value�ĸ���

    for (i = 0; i < max_count; ++i)
    {
        strncpy(values[i], reply->element[i]->str, VALUES_ID_SIZE-1);
        values[i][VALUES_ID_SIZE-1] = 0; //������
    }

END:
    if(reply != NULL)
    {
        freeReplyObject(reply);
    }

    return retn;
}

/* -------------------------------------------*/
/**
 * @brief		��ָ����zset����Ӧ�ĳ�Ա��ֵ����1
 *				��key �� ��Ա������ �򴴽���
 *
 * @param conn		�ѽ���������
 * @param key		zset����
 * @param member	zset��Ա��
 *
 * @returns
 *			0			succ
 *			-1			fail
 */
/* -------------------------------------------*/
int rop_zset_increment(redisContext *conn, char* key, char* member)
{
    int retn = 0;

    redisReply *reply = NULL;

    reply = redisCommand(conn, "ZINCRBY %s 1 %s", key, member);
    //rop_test_reply_type(reply);
    if (strcmp(reply->str, "OK") != 0)
    {
       make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS]Add or increment table:member Error\n");

        retn = -1;
        goto END;
    }

END:
    freeReplyObject(reply);
    return retn;
}

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
int rop_zset_zcard(redisContext *conn, char *key)
{
    int cnt = 0;

    redisReply *reply = NULL;

    reply = redisCommand(conn, "ZCARD %s", key);
    if (reply->type != REDIS_REPLY_INTEGER)
    {
       make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS]ZCARD error \n");
        cnt = -1;
        goto END;
    }

    cnt = reply->integer;

END:
    freeReplyObject(reply);
    return cnt;
}


/* -------------------------------------------*/
/**
 * @brief        �õ�zsetһ��member��score
 *
 * @param conn        �ѽ���������
 * @param key        zset����
 * @param member    zset��Ա��
 *
 * @returns
 *            >=0            succ
 *            -1            fail
 */
/* -------------------------------------------*/
int rop_zset_get_score(redisContext *conn, char *key, char *member)
{
    int score = 0;

    redisReply *reply = NULL;

    reply = redisCommand(conn, "ZSCORE %s %s", key, member);
    rop_test_reply_type(reply);

    if (reply->type != REDIS_REPLY_STRING) {
       make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS]ZSCORE error\n");
        score = -1;
        goto END;
    }

    score = atoi(reply->str);


END:
    freeReplyObject(reply);

    return score;
}

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
int rop_zset_exit(redisContext *conn, char *key, char *member)
{
    int retn = 0;
    redisReply *reply = NULL;

    //ִ������
    reply = redisCommand(conn, "zlexcount %s [%s [%s", key, member, member);
    //rop_test_reply_type(reply);

    if (reply->type != REDIS_REPLY_INTEGER)
    {
       make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS]zlexcount:member Error\n");
        retn = -1;
        goto END;
    }

    retn = reply->integer;

END:

    freeReplyObject(reply);
    return retn;
}

/* -------------------------------------------*/
/**
 * @brief	 ������ָ����zset����Ӧ�ĳ�Ա��ֵ����1
 *				��key �� ��Ա������ �򴴽���
 *
 * @param conn		�ѽ����õ�����
 * @param key		���򼯺�����
 * @param values	��װ�õĳ�Ա����
 * @param val_num	���ݸ���
 *
 * @returns
 *			0		succ
 *			-1		FAIL
 */
/* -------------------------------------------*/
int rop_zset_increment_append(redisContext *conn, char *key, RVALUES values, int val_num)
{
    int retn = 0;
    int i = 0;
    redisReply *reply = NULL;

    /* �����������ܵ� */
    for (i = 0; i < val_num; ++i) {
        retn = redisAppendCommand(conn, "ZINCRBY %s 1 %s", key, values[i]);
        if (retn != REDIS_OK) {
           make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS]ZINCRBY ERROR!\n");
            retn = -1;
            goto END;
        }
        retn = 0;
    }

    /* �ύ���� */
    for (i = 0; i < val_num; ++i) {
        retn = redisGetReply(conn, (void**)&reply);
        if (retn != REDIS_OK) {
            retn = -1;
            make_log(redis_log, ZLOG_LEVEL_ERROR, "[-][GMS_REDIS]Commit ZINCRBY  ERROR!\n");
            freeReplyObject(reply);
            break;
        }
        freeReplyObject(reply);
        retn = 0;
    }

END:
    return retn;
}
