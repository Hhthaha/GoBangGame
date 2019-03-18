#include "game_home.h"

/////////////////////////////////////////////////////
// PKHome
/////////////////////////////////////////////////////

void PKHomeInit(PKHome* phead)
{
  if (phead == NULL) {
    return;
  }
  phead->next = phead;
  phead->prev = phead;
}

PKHome* PKHomeFindSeat(PKHome* head) 
{
  if (head == NULL) {
    return NULL;
  }
  PKHome* cur = head->next;
  while (cur != head) {
    if ((cur->p1 == NULL && cur->p2 == NULL) || 
        (cur->p1 == NULL && cur->p2 != NULL && cur->p2->s == READY) || 
        (cur->p1 != NULL && cur->p1->s == READY && cur->p2 == NULL)) {
      return cur;
    }
    cur = cur->next;
  }
  return NULL;
}

PKHome* CreateSeat(PKHome* phead)
{
  if (phead == NULL) {
    return NULL;
  }

  PKHome* new_node = (PKHome*)malloc(sizeof(PKHome));
  new_node->next = new_node;
  new_node->prev = new_node;
  new_node->p1 = NULL;
  new_node->p2 = NULL;

  new_node->next = phead;
  new_node->prev = phead->prev;
  phead->prev->next = new_node;
  phead->prev= new_node;

  return new_node;
}

void PKHomeDestroy(PKHome* phead, PKHome* seat)
{
  if (phead == NULL || phead->next == phead) {
    return;
  }

  seat->next->prev = seat->prev;
  seat->prev->next = seat->next;
  free(seat);
  seat = NULL;

  return;
}


