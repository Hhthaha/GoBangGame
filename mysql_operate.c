#include "mysql_operate.h"
#include "comm.h"



int InsertSql(Local* l, MYSQL* m_conn)
{
  char sql[SQL_LENGTH];//定义一个SQL_LENGTH的字符串
  mysql_init(m_conn);//初始化m_conn
  if(!mysql_real_connect(m_conn,"localhost","root","","Game",3306,NULL,0))//连接MySQL
  {
    printf("connect\n");
    perror("mysql_real_connect");
    return -1;//链接失败
  }
  sprintf(sql,"insert into GameInfo values('%s','%s',%d);",l->id_buf,l->passwd_buf,1200);//输入到sql里
  if(mysql_query(m_conn,sql))//此处的操作是插入 
  {
    perror("mysql_query");
    return -1;//链接失败
  }
  //释放结果集
  mysql_close(m_conn);
  return 0;
}

int RegistSelectSql(Local* l, MYSQL* m_conn)
{
  char sql[SQL_LENGTH];
  mysql_init(m_conn);
  if(!mysql_real_connect(m_conn,"localhost","root","","Game",3306,NULL,0))
  {
    printf("Select     connect\n");
    perror("mysql_real_connect");
    return -1;//链接失败
  }
  sprintf(sql,"select * from GameInfo where ID='%s';",l->id_buf);
  if(mysql_query(m_conn,sql))//查询
  {
    perror("mysql_query");
    return 1;//没查到 返回1
  }
  
  //获取行数
  /*MYSQL_RES *result = mysql_store_result(m_conn);
  //获取列数                                                                                                                                    
  my_ulonglong rows = mysql_num_rows(result);//获取查询的行数
  MYSQL_ROW line;

  size_t i = 0;
  for(i = 0; i < rows ; ++i)
  {   
    line =  mysql_fetch_row(result);//取每一行
    unsigned int j = 0;
    if(strcmp(line[j], l->id_buf) == 0)//比较每一列的
    {
      return 0;
    }
  } */  

  mysql_close(m_conn);
  return 0;
}

int LoginSelectSql(Local* l, MYSQL* m_conn)
{
  char sql[SQL_LENGTH];
  mysql_init(m_conn);
  if(!mysql_real_connect(m_conn,"localhost","root","","Game",3306,NULL,0))
  {
    printf("Select     connect\n");
    perror("mysql_real_connect");
    return -1;//链接失败
  }
  sprintf(sql,"select * from GameInfo where ID='%s';",l->id_buf);
  if(mysql_query(m_conn,sql))//查询
  {
    perror("mysql_query");
    return -1;
  }
  
  //获取行数
  MYSQL_RES *result = mysql_store_result(m_conn);
  //获取列数                                                                                                                                    
  my_ulonglong rows = mysql_num_rows(result);
  MYSQL_ROW line;

  size_t i = 0;
  for(i = 0; i < rows ; ++i)
  {   
    line =  mysql_fetch_row(result);
    unsigned int j = 0;
    if(strcmp(line[j],l->id_buf)== 0 && strcmp(line[1], l->passwd_buf) == 0)
    {
	printf("找到用户！");   
      return atoi(line[2]);
    }
    printf("未找到用户");
  }   

  mysql_close(m_conn);
  return -1;
}



