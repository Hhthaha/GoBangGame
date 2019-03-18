#pragma once

#include "game.h"

void PersonInit(Person* phead); // 初始化

Person* PersonCreate(Local* l); // 创建

void PersonPush(Person* phead,Local* l); // 添加

Person* FindLocalPerson(Person* head, Local* l); // 查询
