#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>

int Usage(int argc, char* argv[]); // Usage 用法

int readConfig(char buf[], char* path);

int ServerStartUp(int port); // StartUp 监听套接字创建 绑定等

int ClientStartUp(int port); // StartUp 监听套接字创建 绑定等

int64_t GetConnect(int listen_sock, struct sockaddr* client, socklen_t len); // 获取连接，得到 sock

void EchoErrorLog(); // 日志打印
