#pragma once

#include "game.h"

#include "person_info.h"

void GameInfInit(GameInf* phead);

GameInf* GameInfCreate(Local* l, int64_t sock, Person* head_person);

void GameInfPush(GameInf* phead, Local* l, int64_t sock, Person* head_person);

GameInf* GameInfFind(GameInf* head, int64_t sock);

void GameInfDestroy(GameInf* phead, GameInf* pos);

GameInf* AddReadyQueue(GameInf* phead_ready, GameInf* phead_stay, int64_t sock);

GameInf* AddStayQueue(GameInf* phead_stay, GameInf* phead_ready, int64_t sock);

GameInf* LoginFind(GameInf* head, Local* l);
