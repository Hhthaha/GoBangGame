#include "comm.h"

int readConfig(char buf[], char* path)
{
  if (buf == NULL) {
    return -1;
  }
  int fd = open(path, O_RDONLY);
  if (fd < 0) {
    perror("open");
    return -1;
  }

  char c = '\0';
  int i = 0;
  int flag = 0;
  while (1) {
    read(fd, &c, 1);
    if (c == '\n') {
      c = '\0';
      flag++;
      if (flag == 2) {
        break;
      }
    }
    buf[i++] = c;
  }
  return i;
}

// Usage
int Usage(int argc, char* argv[]) 
{
  if (argc < 3) {
    printf("Usage: %s [port] \n",argv[0]);
    return 0;
  }

  return 1;
}

int ServerStartUp(int port) // StartUp 监听套接字创建 绑定等
{
  int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_sock < 0) {
    perror("socket");
    exit(2);
  }

  int opt = 1;
  
  setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  struct sockaddr_in local;
  bzero(&local, sizeof(local));
  local.sin_family = AF_INET;
  local.sin_addr.s_addr = INADDR_ANY;
  local.sin_port = htons(port);

  if (bind(listen_sock, (struct sockaddr*)&local, sizeof(local)) < 0) {
    perror("bind");
    exit(3);
  }
  
  if (listen(listen_sock, 5) < 0) {
    perror("listen");
    exit(4);
  }

  return listen_sock;
}

int ClientStartUp(int port) // StartUp 监听套接字创建 绑定等
{
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("socket");
    exit(2);
  }

  struct sockaddr_in local;
  bzero(&local, sizeof(local));
  local.sin_family = AF_INET;
  local.sin_addr.s_addr = INADDR_ANY;
  local.sin_port = htons(port);
  socklen_t len = sizeof(local);

  if (connect(sock, (struct sockaddr*)&local, len) < 0) {
    perror("connect");
    exit(3);
  }

  return sock;
}


int64_t GetConnect(int listen_sock, struct sockaddr* client, socklen_t len) // 获取连接，得到 sock
{
  int sock = accept(listen_sock, (struct sockaddr*)&client, &len);
  if (sock < 0) {
    perror("accept");
    exit(5);
  }

  return sock;
}

void EchoErrorLog()
{
  time_t timep;
  struct tm *p;
  time (&timep);
  p=gmtime(&timep);

  char buf[64];
  sprintf(buf, "%2d-%02d-%2d  %02d:%02d:%02d: ", 1900+p->tm_year, 1+p->tm_mon, p->tm_mday, 8+p->tm_hour, p->tm_min, p->tm_sec);
  write(2, buf, strlen(buf));
}
