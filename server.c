#include <signal.h>
#include "comm.h"
#include "mysql_operate.h"
#include "game.h"
#include "game_info.h"
#include "game_home.h"
#include "person_info.h"
#include "thread_pool.h"

MYSQL sql;

Person head_person; // 游戏内信息链表头
GameInf head_stay; // stay 队列
GameInf head_ready; // ready 队列

PKHome normal_head_pkhome; // normal_home 
PKHome _1000_1200_head_pkhome; // 1000 - 1200 分段
PKHome _1201_1400_head_pkhome; // 1201 - 1400 分段

pthread_mutex_t person_mutex; // 个人信息互斥量
pthread_mutex_t ready_mutex; // 就绪队列互斥量
pthread_mutex_t stay_mutex; // 游戏大厅互斥量
pthread_mutex_t pkhome_mutex; // 游戏房间互斥量
pthread_mutex_t _1200_pkhome_mutex; // 游戏房间互斥量
pthread_mutex_t _1201_pkhome_mutex; // 游戏房间互斥量

#define ALIVETIME 10 // 心跳包每 10 s 发送一次
#define ALIVEINTVL 5 // 如果没有接收到返回，那么接下来每 5 s 发送一次
#define ALIVEPROBES 3 // 总共发 3 次

#define DEFAULT_CONFIG "default.config"

typedef struct CommonSock {
  int handler_sock;
  int kpalive_sock;
} cSock;

static void* KeepAliveRequest(void* arg) // 心跳包机制
{
  struct sigaction sa;
  sa.sa_handler = SIG_IGN;
  sa.sa_flags = 0;
  sigaction(SIGPIPE, &sa, 0);
  cSock* cs = (cSock*)arg;
  int sock = cs->kpalive_sock;
  char send_buf[32] = "Are you alive?";

  while (1) {
    sleep(ALIVETIME);
    write(sock, send_buf, strlen(send_buf));
    ssize_t s = read(sock, send_buf, sizeof(send_buf) - 1);
    if (s == 0) {
      int count = 1;
      while (1) { // Mark beg
        sleep(ALIVEINTVL);
        write(sock, send_buf, strlen(send_buf));
        s = read(sock, send_buf, sizeof(send_buf) - 1);
        count++;
        if (s > 0) {
          break;
        }
        if (count >= ALIVEPROBES) {
          GameInf* ret_stay = GameInfFind(&head_stay, sock);
          GameInf* ret_ready = GameInfFind(&head_ready, sock);

          pthread_mutex_lock(&ready_mutex);
          GameInfDestroy(&head_ready, ret_ready);
          pthread_mutex_unlock(&ready_mutex);

          pthread_mutex_lock(&stay_mutex);
          GameInfDestroy(&head_stay, ret_stay);
          pthread_mutex_unlock(&stay_mutex);

          close(cs->handler_sock);
          close(sock);
          free(cs);
          return NULL;
        }
      } // Mark end while (1)
    }
  }
  return NULL;
}

static void* HandlerRequest(void* arg) // 主执行
{
  int64_t sock = (int64_t)arg;
  ssize_t s = 0;

  PKHome* seat = NULL;

  GameInf* ret_stay = NULL; 
  GameInf* ret_ready = NULL; 
  Local l;

  while (1) { // 处理登录界面的请求
    s = read(sock, &l, sizeof(l));
    if (s <= 0) {
      if (s < 0) {
        EchoErrorLog();
        perror("read");
      }
      goto end;
    }

    if (l.ls == LOGIN) { // 登录
      int ret = LoginSelectSql(&l, &sql);
      GameInf* ret_stay = LoginFind(&head_stay, &l);
      GameInf* ret_ready = LoginFind(&head_ready, &l);
      if (ret != -1 &&  
          ret_stay == NULL && ret_ready == NULL) {
        l.score = ret;
        write(sock, "yes", 3);

        pthread_mutex_lock(&person_mutex);
        PersonPush(&head_person, &l);
        pthread_mutex_unlock(&person_mutex);

        pthread_mutex_lock(&stay_mutex);
        GameInfPush(&head_stay, &l, sock, &head_person);
        pthread_mutex_unlock(&stay_mutex);
        break;
      } else {
          write(sock, "login", 5);
        }
      }
     else if(l.ls == REGISTE) { // 注册
      int ret = RegistSelectSql(&l, &sql);
      if (ret == 0 ) {
        write(sock, "no", 2);
      } else if (ret == 1) {
        write(sock, "yes", 3);

        InsertSql(&l, &sql);
        l.score = 1200;

        pthread_mutex_lock(&person_mutex);
        PersonPush(&head_person, &l);
        pthread_mutex_unlock(&person_mutex);

        pthread_mutex_lock(&stay_mutex);
        GameInfPush(&head_stay, &l, sock, &head_person);
        pthread_mutex_unlock(&stay_mutex);
        break;
      } else {
        EchoErrorLog();
        perror("SelectSql Error!");
        goto end;
      }
    } else { // 退出
      goto end;
    }
  }

  Coord c; // 定义坐标
q:while (1) {
    s = read(sock, &l, sizeof(l)); // 游戏是否开始
    if (s <= 0) {
      if (s < 0) {
        EchoErrorLog();
        perror("read");
      }
      goto end;
    }

    if (l.ls == EXIT) { // 退出
      goto end;
    }

    pthread_mutex_lock(&stay_mutex);
    pthread_mutex_lock(&ready_mutex);
    GameInf* person_node = AddReadyQueue(&head_ready, &head_stay, sock);
    person_node->s = READY;
    pthread_mutex_unlock(&stay_mutex);
    pthread_mutex_unlock(&ready_mutex);

    int choose = 0;
    s = read(sock, &choose, sizeof(choose)); // 游戏模式选择
    if (s <= 0) {
      if (s < 0) {
        EchoErrorLog();
        perror("read");
      }
      goto end;
    }

    if (choose == 1) { // normal
      seat = PKHomeFindSeat(&normal_head_pkhome);
      pthread_mutex_lock(&pkhome_mutex);
      if (seat == NULL) {
        seat = CreateSeat(&normal_head_pkhome);
        seat->p1 = person_node;
      } else {
        if (seat->p1 == NULL) {
          seat->p1 = person_node;
        } else if (seat->p2 == NULL){
          seat->p2 = person_node;
        }
      }
      pthread_mutex_unlock(&pkhome_mutex);
    } else if (choose == 2) {
      int score = person_node->p->score;

      if (score <= 1200) {
        seat = PKHomeFindSeat(&_1000_1200_head_pkhome);
        pthread_mutex_lock(&_1200_pkhome_mutex);
        if (seat == NULL) {
          seat = CreateSeat(&_1000_1200_head_pkhome);
          seat->p1 = person_node;
        } else {
          if (seat->p1 == NULL) {
            seat->p1 = person_node;
          } else if (seat->p2 == NULL){
            seat->p2 = person_node;
          }
        }
        pthread_mutex_unlock(&_1200_pkhome_mutex);
      } else if (score >= 1201 && score <= 1400) {
        seat = PKHomeFindSeat(&_1201_1400_head_pkhome);
        pthread_mutex_lock(&_1201_pkhome_mutex);
        if (seat == NULL) {
          seat = CreateSeat(&_1201_1400_head_pkhome);
          seat->p1 = person_node;
        } else {
          if (seat->p1 == NULL) {
            seat->p1 = person_node;
          } else if (seat->p2 == NULL){
            seat->p2 = person_node;
          }
        }
        pthread_mutex_unlock(&_1201_pkhome_mutex);
      }
    }
    printf("seat is %p\n", seat);

    while (1) { // Mark:beg
      while (seat->p1 == NULL || seat->p2 == NULL) {
      }

      if (seat->p1 != NULL && seat->p1->sock == sock) {
        seat->p1->s = PLAY;
      }
      if (seat->p2 != NULL && seat->p2->sock == sock) {
        seat->p2->s = PLAY;
      }

      while (1) {
        if (seat->p1 != NULL && seat->p2 != NULL) {
          if (seat->p1->s == PLAY && seat->p2->s == PLAY) {
            break;
          }
        }
      }
            

      if (seat->p1->sock == sock) {
        write(seat->p1->sock, "first", 5);
      } 
      if (seat->p2->sock == sock) {
        write(seat->p2->sock, "second", 6);
      }

      c.win_sock = 0;
      c.flag = 'a';
      c.who = 0;

      while (1) {
        if (seat->p1 != NULL && seat->p1->sock == sock){
          s = read(seat->p2->sock, &c, sizeof(c));//读取玩家二发来的坐标
          if (s <= 0) {
            if (s < 0) {
              EchoErrorLog();
              perror("read");
            }
            AddStayQueue(&head_stay, &head_ready, sock);
            c.flag = 'q';
            write(seat->p1->sock, &c, sizeof(c));
            goto q;
          }

          write(seat->p1->sock, &c, sizeof(c));//把玩家二下的坐标发给玩家一
          if (c.win_sock != 0) {
            if (c.who == 1) {
              person_node->p->score += ADDSCORE;
            } else if (c. who == 2){
              person_node->p->score -= SUBSCORE;
            } else {
              person_node->p->score += DRAWSCORE;
            }
            seat->p1->s = FINISH;
            seat->p2->s = FINISH;
            break;
          }
        } 

        if (seat->p1 != NULL && seat->p2->sock == sock) {
          s = read(seat->p1->sock, &c, sizeof(c));
          if (s <= 0) {
            if (s < 0) {
              EchoErrorLog();
              perror("read");
            }
            AddStayQueue(&head_stay, &head_ready, sock);
            c.flag = 'q';
            write(seat->p2->sock, &c, sizeof(c));
            goto q;
          }

          write(seat->p2->sock, &c, sizeof(c));
          if (c.win_sock != 0) {
            if (c.who == 2) {
              person_node->p->score += ADDSCORE;
            } else if (c. who == 1){
              person_node->p->score -= SUBSCORE;
            } else {
              person_node->p->score += DRAWSCORE;
            }
            seat->p1->s = FINISH;
            seat->p2->s = FINISH;
            break;
          }
        } 
      } // end while (1)

      GpNode p;
      strcpy(p.id, person_node->p->id);
      p.score = person_node->p->score;
      if (c.who == 1) {
        if (sock == seat->p1->sock) {
          p.win = 1;
        } else {
          p.win = 2;
        }
      } else if (c.who == 2) {
        if (sock == seat->p1->sock) {
          p.win = 2;
        } else {
          p.win = 1;
        }
      } else {
        p.win = 0;
      }
      write(sock, &p, sizeof(p));

      s = read(sock, &l, sizeof(l));//获取此时客户端的状态
      if (s <= 0) {
        if (s < 0) {
          EchoErrorLog();
          perror("read");
        }
        goto end;
      }

      if (l.ls == EXIT) { // 退出游戏
        if (seat->p1 != NULL && seat->p1->sock == sock) {
          write(seat->p1->sock, "ok", 2);
        } 
        if (seat->p2 != NULL && seat->p2->sock == sock) {
          write(seat->p2->sock, "ok", 2);
        }
        goto end;
      } else if (l.ls == RETURN) { // 返回游戏大厅
        if (seat->p1 != NULL && seat->p1->sock == sock) {
          seat->p1->s = STAY;
          seat->p1 = NULL;
        } else {
          seat->p2->s = STAY;
          seat->p2 = NULL;
        }
        write(sock, "ok", 2);

        pthread_mutex_lock(&stay_mutex);
        pthread_mutex_lock(&ready_mutex);
        AddStayQueue(&head_stay, &head_ready, sock);
        pthread_mutex_unlock(&stay_mutex);
        pthread_mutex_unlock(&ready_mutex);

        break;
      } else { // 继续游戏
        if (seat->p1 != NULL && seat->p1->sock == sock) {
          seat->p1->s = READY;
          write(seat->p1->sock, "ok", 2);
        } 
        if (seat->p2 != NULL && seat->p2->sock == sock) {
          seat->p2->s = READY;
          write(seat->p2->sock, "ok", 2);
        }
      }
    } // end while (1) Mark:beg
    // if (seat->p1 == NULL && seat->p2 == NULL) {
    //   PKHomeDestroy(&normal_head_pkhome, seat);
    // }
  }

end:
  ret_stay = GameInfFind(&head_stay, sock);
  ret_ready = GameInfFind(&head_ready, sock);

  pthread_mutex_lock(&stay_mutex);
  GameInfDestroy(&head_stay, ret_stay);
  pthread_mutex_unlock(&stay_mutex);

  pthread_mutex_lock(&ready_mutex);
  GameInfDestroy(&head_ready, ret_ready);
  pthread_mutex_unlock(&ready_mutex);
  close(sock);

  return NULL;
}

int main()
{
  char config_path[] = DEFAULT_CONFIG;
  char buf[64];
  char port1[5] = {0};
  char port2[5] = {0};
  
  int ret = readConfig(buf, config_path);
  if (ret < 0) {
    return 1;
  }
  strcpy(port1, buf + 6);
  strcpy(port2, buf + 17);

  struct sigaction sa;
  sa.sa_handler = SIG_IGN;
  sa.sa_flags = 0;
  sigaction(SIGPIPE, &sa, 0);

  int fd = open("Error.log", O_RDWR | O_CREAT | O_APPEND, 0666);
  dup2(fd, 2);

  ThreaddPool tp;
  ThreadPoolInit(&tp); // 初始化线程池

  int handler_listen_sock = ServerStartUp(atoi(port1));
  int kpalive_listen_sock = ServerStartUp(atoi(port2));

  PersonInit(&head_person);
  GameInfInit(&head_stay);
  GameInfInit(&head_ready);
  PKHomeInit(&normal_head_pkhome);
  PKHomeInit(&_1000_1200_head_pkhome);
  PKHomeInit(&_1201_1400_head_pkhome);

  pthread_mutex_init(&stay_mutex, NULL);
  pthread_mutex_init(&person_mutex, NULL);
  pthread_mutex_init(&ready_mutex, NULL);
  pthread_mutex_init(&pkhome_mutex, NULL);
  pthread_mutex_init(&_1200_pkhome_mutex, NULL);
  pthread_mutex_init(&_1201_pkhome_mutex, NULL);

  while (1) {
    struct sockaddr_in client_1; // 为了客户端主执行任务
    socklen_t len1 = sizeof(client_1);

    struct sockaddr_in client_2; // 为了心跳包的执行任务
    socklen_t len2 = sizeof(client_2);

    int64_t handler_sock = GetConnect(handler_listen_sock, (struct sockaddr*)&client_1, len1);
    int64_t kpalive_sock = GetConnect(kpalive_listen_sock, (struct sockaddr*)&client_2, len2);

    cSock* cs = (cSock*)malloc(sizeof(cSock));
    cs->handler_sock = handler_sock;
    cs->kpalive_sock = kpalive_sock;

     AddWorkQueue(&tp, KeepAliveRequest, (void*)cs);
     DilationThread(&tp);

    pthread_t tid1;
    //pthread_t tid2;
    pthread_create(&tid1, NULL, HandlerRequest, (void*)handler_sock); // 主执行任务
   // pthread_create(&tid2, NULL, KeepAliveRequest, (void*)cs); // 心跳任务
    pthread_detach(tid1);
    //pthread_detach(tid2);
  }

  ThreaddPoolDestroy(&tp);

  pthread_mutex_destroy(&stay_mutex);
  pthread_mutex_destroy(&ready_mutex);
  pthread_mutex_destroy(&person_mutex);
  pthread_mutex_destroy(&pkhome_mutex);
  pthread_mutex_destroy(&_1200_pkhome_mutex);
  pthread_mutex_destroy(&_1201_pkhome_mutex);

  return 0;
}
