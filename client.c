#include <stdio.h>
#include "comm.h"
#include "game.h"

GpNode p;

#define PORT_1 9999
#define PORT_2 8888

void LoginInterface(int64_t sock)
{
  WelcomeMenu();

  while (1) {
    Local l;
    char check[10];
    LoginMenu();
    char input[3];
    fflush(0);
    scanf("%s",input);

    if (strcmp(input, "1") == 0) {
      // 登录
      Login(&l);
      write(sock, &l, sizeof(l));
      ssize_t s = read(sock, check, sizeof(check) - 1);
      check[s] = '\0';
      if (strcmp("yes", check) == 0) {
        printf("\n*****************************************\n");
        printf("*******        登录成功!         ********\n");
        printf("*****************************************\n");
        break;
      } else if(strcmp("login", check) == 0) {
        printf("\n*****************************************\n");
        printf("****    登录失败，请重新选择!        ****\n");
        printf("*****************************************\n");

      } else {
        printf("\n*****************************************\n");
        printf("****      密码错误，请重新选择!      ****\n");
        printf("*****************************************\n");
      }
    } else if (strcmp(input, "2") == 0) {
      // 注册
      Registe(&l);
      write(sock, &l, sizeof(l));
      ssize_t s = read(sock, check, sizeof(check) - 1);
      check[s] = '\0';
      if (strcmp("yes", check) == 0) {
        printf("\n*****************************************\n");
        printf("*********         注册成功!!       ******\n");
        printf("*****      请记住你的账号与密码!!     ***\n");
        printf("*****************************************\n");
        break;
      } else {
        printf("\n*****************************************\n");
        printf("***       注册失败，请重新选择!       ***\n");
        printf("*****************************************\n");
      }
    } else if(strcmp(input, "0") == 0) {
      Exit(&l);
      write(sock, &l, sizeof(l));
      close(sock);
      exit(0);
    } else {
      printf("\n*****************************************\n");
      printf("***       选择错误!请重新选择!        ***\n");
      printf("*****************************************\n");
    }
  } // end while (1)
}

void PlayGame(int sock)
{
  Local l;
  char buf[12] = {0};
  ssize_t s = 0;
  char board[ROW][COL];
  char check[3] = {0};

  Coord c;
q:while (1) { // Mark: start
    InitBoard(board, ROW, COL);

    PlatFormMenu();
    int input = 0;
    scanf("%d",&input);

    if (input == 1) {
      l.ls = LOGIN;
      write(sock, &l, sizeof(l));
    } else if (input == 0) {
      Exit(&l);
      write(sock, &l, sizeof(l));
      close(sock);
      exit(0);
    } else {
      printf("\n*****************************************\n");
      printf("***       选择错误!请重新选择!        ***\n");
      printf("*****************************************\n");
      goto q;
    }

    while (1) {
      ChooseMode();
      scanf("%d", &input);
      write(sock, &input, sizeof(input));
      if (input == 1) {
        printf("\n*****************************************\n");
        printf("***    选择成功!匹配对战即将开始!     ***\n");
        printf("*****************************************\n");
        break;
      } else if (input == 2) {
        printf("\n*****************************************\n");
        printf("***    选择成功!排位对战即将开始!     ***\n");
        printf("*****************************************\n");
        break;
      } else {
        printf("\n*****************************************\n");
        printf("***       选择错误!请重新选择!        ***\n");
        printf("*****************************************\n");
      }
    }

ret:  printf("\n*****************************************\n");
      printf("*******     等待其他玩家的加入!   *******\n");
      printf("*****************************************\n");

      memset(buf, 0, sizeof(buf));
      s = read(sock, buf, sizeof(buf) - 1);

      c.win_sock = 0;

      if (strcmp("first", buf) == 0) { // first 先落子

        InitBoard(board, ROW, COL);

        printf("\n*****************************************\n");
        printf("************    游戏开始!    ************\n");
        printf("*****************************************\n");

        printf("你先手! 棋子为 X \n");

        while (1) {
          Printf_Board(board, ROW, COL); 
          EnterCoord(&c, board, ROW, COL);
          PlayerOneMove(&c, board);
          Printf_Board(board, ROW, COL);//走一就检查是否获胜，因为这时候完全有可能走完一步就赢了。 
          char win = CheckWin(board, ROW, COL, &c);
          if (win != ' ') {// 已经出结果了
            if (win == 'X') {
              printf("\n*****************************************\n");
              printf("*********** 恭喜你!赢得了比赛! **********\n");
              printf("*****************************************\n");
              c.who = 1;
            } else if (win == 'O') {
              printf("\n*****************************************\n");
              printf("************ 真是遗憾!你输了! ***********\n");
              printf("*****************************************\n");
              c.who = 2;
            } else {
              printf("\n*****************************************\n");
              printf("************      平局!!     ************\n");
              printf("*****************************************\n");
              c.who = 0;
            }
            c.win_sock = sock;
            write(sock, &c, sizeof(c));//把自己刚走的坐标发到服务器
            read(sock, &c, sizeof(c));//读取服务器发来的另外一个玩家的坐标信息
            if (c.flag == 'q') {
              printf("\n************************************\n");
              printf("***对方已经放弃本局游戏,游戏退出!***\n");
              printf("************************************\n");
              goto q;//继续回到选择登录界面
	      }
            break;
          }
          write(sock, &c, sizeof(c));//如果自己没有获胜，就把坐标发到服务器

          printf("等待对手落子!\n");

          read(sock, &c, sizeof(c));//读取另外一个玩家的坐标
          if (c.flag == 'q') {
            printf("\n************************************\n");
            printf("***对方已经放弃本局游戏,游戏退出!***\n");
            printf("************************************\n");
            goto q;
          }
          PlayerTwoMove(&c, board);
          Printf_Board(board, ROW, COL); 
          win = CheckWin(board, ROW, COL, &c);
          if (win != ' ') {
            if (win == 'X') {
              printf("\n*****************************************\n");
              printf("*********** 恭喜你!赢得了比赛! **********\n");
              printf("*****************************************\n");
              c.who = 1;
            } else if (win == 'O') {
              printf("\n*****************************************\n");
              printf("************ 真是遗憾!你输了! ***********\n");
              printf("*****************************************\n");
              c.who = 2;
            } else {
              printf("\n*****************************************\n");
              printf("************      平局!!     ************\n");
              printf("*****************************************\n");
              c.who = 0;
            }
            write(sock, &c, sizeof(c));
            break;
          }
        } // end while (1)
      } 

      if (strcmp(buf, "second") == 0) { // second 后落子 

        InitBoard(board, ROW, COL);

        printf("\n*****************************************\n");
        printf("************    游戏开始!    ************\n");
        printf("*****************************************\n");

        printf("你后手，棋子为 O \n");
        Printf_Board(board, ROW, COL); 

        while (1) {
          printf("等待对手落子!\n");
          read(sock, &c, sizeof(c));
          if (c.flag == 'q') {
            printf("\n************************************\n");
            printf("***对方已经放弃本局游戏,游戏退出!***\n");
            printf("************************************\n");
            goto q;
          }
          PlayerOneMove(&c, board);
          Printf_Board(board, ROW, COL); 
          char win = CheckWin(board, ROW, COL, &c);
          if (win != ' ') {
            if (win == 'X') {
              printf("\n*****************************************\n");
              printf("************ 真是遗憾!你输了! ***********\n");
              printf("*****************************************\n");
              c.who = 1;
            } else if (win == 'O') {
              printf("\n*****************************************\n");
              printf("*********** 恭喜你!赢得了比赛! **********\n");
              printf("*****************************************\n");
              c.who = 2;
            } else {
              printf("\n*****************************************\n");
              printf("************      平局!!     ************\n");
              printf("*****************************************\n");
              c.who = 0;
            }
            write(sock, &c, sizeof(c));
            break;
          }

          EnterCoord(&c, board, ROW, COL);
          PlayerTwoMove(&c, board);
          Printf_Board(board, ROW, COL); 
          win = CheckWin(board, ROW, COL, &c);
          if (win != ' ') {
            if (win == 'X') {
              printf("\n*****************************************\n");
              printf("************ 真是遗憾!你输了! ***********\n");
              printf("*****************************************\n");
              c.who = 1;
            } else if (win == 'O') {
              printf("\n*****************************************\n");
              printf("*********** 恭喜你!赢得了比赛! **********\n");
              printf("*****************************************\n");
              c.who = 2;
            } else {
              printf("\n*****************************************\n");
              printf("************      平局!!     ************\n");
              printf("*****************************************\n");
              c.who = 0;
            }
            c.win_sock = sock;
            write(sock, &c, sizeof(c));
            read(sock, &c, sizeof(c));
            if (c.flag == 'q') {
              printf("\n************************************\n");
              printf("***对方已经放弃本局游戏,游戏退出!***\n");
              printf("************************************\n");
              goto q;
            }
            break;
          }
          write(sock, &c, sizeof(c));
        }
      }

      read(sock, &p, sizeof(p));
      ListPersonInf(&p);

menu: GameMenu();
      scanf("%d",&input);
      switch (input) {
      case 1:
        l.ls = LOGIN;
        write(sock, &l, sizeof(l));
        s = read(sock, check, sizeof(check) - 1);
        goto ret;//回到等待其它玩家进入
      case 2:
        l.ls = RETURN;
        write(sock, &l, sizeof(l));
        read(sock, check, sizeof(check) - 1);
        break;
      case 0:
        Exit(&l);
        write(sock, &l, sizeof(l));
        s = read(sock, check, sizeof(check) - 1);
        close(sock);
        exit(0);
        break;
      default:
        printf("\n*****************************************\n");
        printf("***       选择错误!请重新选择!        ***\n");
        printf("*****************************************\n");
        goto menu;
        break;
      }
  } // end while (1) Mark:start
}

void* HandlerRequest(void* arg)
{
  int64_t sock = (int64_t) arg;
  LoginInterface(sock);

  PlayGame(sock);
  return NULL;
}

void* KeepAliveRequest(void* arg) // 心跳机制
{
  int64_t sock = (int64_t) arg;
  char recv_buf[32] = {0};
  while (1) {
    int s = read(sock, recv_buf, sizeof(recv_buf) - 1);
    if (s <= 0) {
      perror("read");
      break;
    }
    write(sock, "yes", 3);
  }
  return NULL;
}

int main()
{
  int64_t sock = ClientStartUp(PORT_1);
  int64_t kpalive_sock = ClientStartUp(PORT_2);

  pthread_t tid1;
  pthread_t tid2;
  pthread_create(&tid1, NULL, HandlerRequest, (void*)sock);
  pthread_detach(tid1);
  pthread_create(&tid2, NULL, KeepAliveRequest, (void*)kpalive_sock);
  pthread_join(tid2, NULL);

  close(sock);
  return 0;
}
