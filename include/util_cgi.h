#ifndef _UTIL_CGI_H
#define _UTIL_CGI_H

#include <zlog.h>

#define FILE_NAME_LEN     (256) //文件名字长度
#define TEMP_BUF_MAX_LEN  (512)	//临时缓冲区大小
#define USER_NAME_LEN     (128)	//用户名字长度
#define FILE_URL_LEN			(512)	//文件存放url长度
#define PASSWD_LEN				(128)	//用户密码长度
//文件信息
typedef struct FILEINFO
{
		long size; // file size
    int flag; // success or fail
    long len; //content len
    char filename[FILE_NAME_LEN];
    char *fileContent; //内容
    char fileId[TEMP_BUF_MAX_LEN];   //文件上传后fdfs后id
    struct USERINFO *userinfo;
}FileInfo;

//用户信息
typedef struct userInfo
{
    char username[USER_NAME_LEN];
    char passwd[PASSWD_LEN];
}UserInfo;
#endif
