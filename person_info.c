#include "person_info.h"

/////////////////////////////////////////////////////
// Person
/////////////////////////////////////////////////////

void PersonInit(Person* phead) // 初始化
{
  if (phead == NULL) {
    return;
  }
  phead->next = phead;
  phead->prev = phead;
}

Person* PersonCreate(Local* l) // 创建
{
  Person* tmp = (Person*)malloc(sizeof(Person));

  strcpy(tmp->id, l->id_buf);
  strcpy(tmp->passwd, l->passwd_buf);
  tmp->next = NULL;
  tmp->prev = NULL;
  tmp->score = l->score;

  return tmp;
}

void PersonPush(Person* phead,Local* l) // 添加
{
  if (phead == NULL) {
    return;
  }

  Person* new_node = PersonCreate(l);
  new_node->next = phead;
  new_node->prev = phead->prev;
  phead->prev->next = new_node;
  phead->prev = new_node;
  return;
}

Person* FindLocalPerson(Person* head, Local* l) // 查询
{
  Person* cur = head->next;
  while (cur != head) {
    if ((strcmp(cur->id, l->id_buf) == 0) &&
        (strcmp(cur->passwd, l->passwd_buf) == 0)) {
      return cur;
    }
    cur = cur->next;
  }
  return NULL;
}


