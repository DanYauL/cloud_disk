#ifndef _UPLOAD_CGI_H   
#define _UPLOAD_CGI_H
#include "util_cgi.h"

void upload_file(); //上传文件接口
void fcgi_for_dynamic_request(FileInfo *fileInfo); //fcgi处理动态请求获取文件信息
int recv_save_file(FileInfo *fileInfo);
#endif
