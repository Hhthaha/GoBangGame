#include "game_info.h"

/////////////////////////////////////////////////////
// GameInformation
/////////////////////////////////////////////////////

void GameInfInit(GameInf* phead)
{
  if (phead == NULL) {
    return;
  }
  phead->next = phead;
  phead->prev = phead;
}

GameInf* GameInfCreate(Local* l, int64_t sock, Person* head_person)
{
  GameInf* tmp= (GameInf*)malloc(sizeof(GameInf));

  tmp->sock = sock;
  tmp->s = STAY;
  tmp->p = FindLocalPerson(head_person, l);
  tmp->next = NULL;
  tmp->prev = NULL;

  return tmp;
}

void GameInfPush(GameInf* phead, Local* l, int64_t sock, Person* head_person)
{
  if (phead == NULL || l == NULL) {
    return;
  }

  GameInf* new_node = GameInfCreate(l, sock, head_person);
  new_node->next = phead;
  new_node->prev = phead->prev;
  phead->prev->next = new_node;
  phead->prev = new_node;
  printf("%p %p %p\n",phead, new_node->next, new_node->prev);
  return;
}

GameInf* GameInfFind(GameInf* head, int64_t sock)
{
  if (head == NULL) {
    return NULL;
  }
  GameInf* cur = head->next;
  while (cur != head) {
    if (cur->sock == sock) {
      return cur;
    }
    cur = cur->next;
  }
  return NULL;
}

void GameInfDestroy(GameInf* phead, GameInf* pos)
{
  if (phead == NULL || pos == NULL) {
    return;
  }

  pos->next->prev = pos->prev;
  pos->prev->next = pos->next;
  free(pos);
  pos = NULL;

  return;
}

GameInf* AddReadyQueue(GameInf* phead_ready, GameInf* phead_stay, int64_t sock)
{
  if (phead_stay == NULL || phead_ready == NULL) {
    return NULL;
  }
  GameInf* pos = GameInfFind(phead_stay, sock);

  pos->next->prev = pos->prev;
  pos->prev->next = pos->next;

  pos->next = phead_ready;
  pos->prev = phead_ready->prev;
  phead_ready->prev->next = pos;
  phead_ready->prev = pos;

  return pos;
}

GameInf* LoginFind(GameInf* head, Local* l)
{
  if (head == NULL) {
    return NULL;
  }
  GameInf* cur = head->next;
  while (cur != head) {
    if (strcmp(cur->p->id, l->id_buf) == 0) {
      if (strcmp(cur->p->passwd, l->passwd_buf) == 0) {
        return cur;
      }
    }
    cur = cur->next;
  }
  return NULL;
}

GameInf* AddStayQueue(GameInf* phead_stay, GameInf* phead_ready, int64_t sock)
{
  return AddReadyQueue(phead_stay, phead_ready, sock);
}

