#include "game.h"

void WelcomeMenu()
{
  printf("\n");
  printf("************************************\n");
  printf("*****                          *****\n");
  printf("*****  Welcome messi Platform! *****\n");
  printf("*****                          *****\n");
  printf("*****      Have Good Time!     *****\n");
  printf("*****                          *****\n");
  printf("************************************\n");
  printf("*****      By MessiGOAT        *****\n");
  printf("************************************\n");
  printf("\n");
}

void LoginMenu() 
{
  printf("\n");
  printf("************************************\n");
  printf("*********** <游戏菜单>   ***********\n");
  printf("***********              ***********\n");
  printf("************************************\n");
  printf("***********   1.登录     ***********\n");
  printf("***********              ***********\n");
  printf("***********   2.注册     ***********\n");
  printf("***********              ***********\n");
  printf("***********   0.退出游戏 ***********\n");
  printf("************************************\n");
  printf("\n");
  printf("Please Enter Your Choose##  ");
}

void Login(Local* l) // 登录
{
  l->ls = LOGIN;
  printf("\n");
  printf("************************************\n");
  printf("**    请输入你的账户名与密码!     **\n");
  printf("**             <登录>             **\n");
  printf("************************************\n");
  printf("\n");
  printf("账号:");
  fflush(0);//刷新f（0）标准输入
  ssize_t s = read(0, l->id_buf, sizeof(l->id_buf));//从标准输入里面读数据
  l->id_buf[s - 1] = '\0';
  // scanf("%s",l->id_buf);
  printf("密码:");
  fflush(0);
  s = read(0, l->passwd_buf, sizeof(l->passwd_buf));
  l->passwd_buf[s - 1] = '\0';
  // scanf("%s",l->passwd_buf);
}
      
void Exit(Local* l) // 退出
{
  l->ls = EXIT;
  printf("\n");
  printf("************************************\n");
  printf("*****        游戏退出!!        *****\n");
  printf("************************************\n");
  printf("\n");
}

void Registe(Local* l) // 注册
{
  l->ls = REGISTE;
  while (1) {
    printf("\n");
    printf("************************************\n");
    printf("**    请输入你的账户名与密码!     **\n");
    printf("**             <注册>             **\n");
    printf("************************************\n");
    printf("\n");
    printf("账号(少于 16 位):");
    fflush(0);
    ssize_t s = read(0, l->id_buf, sizeof(l->id_buf));
    if (s > 16 ) {
      printf("输入过长，请重新输入!\n");
      continue;
    }
    l->id_buf[s - 1] = '\0';
    printf("密码(少于 16 位):");
    fflush(0);
    s = read(0, l->passwd_buf, sizeof(l->passwd_buf));
    if (s > 16 ) {
      printf("输入过长，请重新输入!\n");
      continue;
    }
    l->passwd_buf[s - 1] = '\0';
    break;
  }
}

void PlatFormMenu() // 游戏大厅界面
{
  printf("\n");
  printf("************************************\n");
  printf("********** <PlatFormMenu>  *********\n");
  printf("***********              ***********\n");
  printf("************************************\n");
  printf("***********  1.开始游戏  ***********\n");
  printf("***********              ***********\n");
  printf("***********    0.退出    ***********\n");
  printf("************************************\n");
  printf("\n");
  printf("请输入你的选择->");
}

void ChooseMode() // 游戏大厅界面
{
  printf("\n");
  printf("************************************\n");
  printf("**********     <MODE>      *********\n");
  printf("***********              ***********\n");
  printf("************************************\n");
  printf("***********  1.普通对战  ***********\n");
  printf("***********              ***********\n");
  printf("***********  2.排位模式  ***********\n");
  printf("************************************\n");
  printf("\n");
  printf("请输入你的选择->");
}

void GameMenu() // 游戏界面
{
  printf("\n");
  printf("************************************\n");
  printf("*******  <游戏结束!请选择!>  *******\n");
  printf("***********              ***********\n");
  printf("************************************\n");
  printf("***********  1.继续游戏  ***********\n");
  printf("***********              ***********\n");
  printf("***********  2.返回大厅  ***********\n");
  printf("***********              ***********\n");
  printf("***********  0.退出      ***********\n");
  printf("************************************\n");
  printf("\n");
  printf("请输入你的选择->");
}

void ListPersonInf(GpNode* p)
{
  if (p == NULL) {
    return;
  }
  printf("\n你的账户 ID 为: %s\n", p->id);
  printf("你的游戏积分为: %d\n", p->score);
  if (p->win == 1) {
    printf("本局游戏胜利 : +%d\n", ADDSCORE);
  } else if (p->win == 0) {
    printf("本局游戏平局 : +%d\n", DRAWSCORE);
  } else {
    printf("本局游戏失败 : -%d\n", SUBSCORE);
  }
  return;
}

void Printf_Board(char board[ROW][COL], int row, int col)//打印棋盘
{
  int i = 0;
  int j = 0;
  printf(" 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 \n");
  printf("---|---|---|---|---|---|---|---|---|---|---\n");
  for (i = 0; i < row; i++) {
    for (j = 0; j < col; j++) {
      if (j == 0 && i < 9) {
        printf(" %d |",i + 1);
      }
      if (j == 0 && i == 9) {
        printf("%2d |",i + 1);
      }
      printf(" %c ",board[i][j]);
      if (j < col - 1) {
        printf("|");   
      } 
    }
    printf("\n");
    if (i < row - 1) {
      printf("---|---|---|---|---|---|---|---|---|---|---\n");
    }
  }
  printf("\n");
  printf("\n");
  return;
}

void InitBoard(char board[ROW][COL], int row, int col)//初始化棋盘
{
  int i = 0;
  int j = 0;
  for (i = 0; i < row; i++) {
    for (j = 0; j < col; j++) {
      board[i][j] = ' ';
    }
  }
  return;
}

void PlayerOneMove(Coord* c, char board[ROW][COL])
{
  board[c->x - 1][c->y - 1] = 'X';
}

void PlayerTwoMove(Coord* c, char board[ROW][COL])
{
  board[c->x - 1][c->y - 1] = 'O';
}

void EnterCoord(Coord* c, char board[ROW][COL], int row, int col)
{
  printf("You Run!\n");
  while (1) {
    printf("enter->");
    scanf("%d%d", &c->x, &c->y);
    if (board[c->x - 1][c->y - 1] == ' ' && c->x >= 1 && 
        c->x <= row && c->y >= 1 && c->y <= col) {
      break;
    }
    else
      printf("你的坐标不合法，请重新输入!\n");
  }
  return;
}

int IsFull(char board[ROW][COL], int row, int col)//判断棋盘是否满
{
  int i = 0;
  int j = 0;
  for (i = 0; i < row; i++) {
    for (j = 0; j < col; j++) {
      if (board[i][j] == ' ') {
        return 0;
      }
    }
  }
  return 1;
}

char CheckWin(char board[ROW][COL], int row, int col, Coord* c)
{
  int x = c->x - 1;
  int y = c->y - 1;

  int full = IsFull(board, row, col);
  if (full == 1) {
    return 'P';
  }
  
  char win = board[x][y];

  // 四种方向进行判断胜负   行 列 两条对角线
  // 行
  int count = 0;
  while (y >= 0 && board[x][y] == win) {
    --y;
  }
  y += 1;
  while (y <= col && board[x][y] == win) {
    ++count;
    ++y;
    if (count == 5) {
      return win;
    }
  }

  // 列
  x = c->x - 1;
  y = c->y - 1;
  count = 0;
  while (x >= 0 && board[x][y] == win) {
    --x;
  }
  x += 1;
  while (x <= row && board[x][y] == win) {
    ++count;
    ++x;
    if (count == 5) {
      return win;
    }
  }

  // 对角线 
  x = c->x - 1;
  y = c->y - 1;
  count = 0;
  while (x >= 0 && y >= 0 && board[x][y] == win) {
    --x;
    --y;
  }
  x += 1;
  y += 1;
  while (x <= row && y <= col && board[x][y] == win) {
    count++;
    ++x;
    ++y;
    if (count == 5) {
      return win;
    }
  }

  x = c->x - 1;
  y = c->y - 1;
  count = 0;
  while (x >= 0 && y <= col && board[x][y] == win) {
    --x;
    ++y;
  }
  x += 1;
  y -= 1;
  while (x <= row && y >= 0 && board[x][y] == win) {
    ++count;
    ++x;
    --y;
    if (count == 5) {
      return win;
    }
  }

  return ' ';
}

