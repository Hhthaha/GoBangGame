#pragma once

#include "/usr/include/mysql/mysql.h"
#include <stdio.h>
#include "game.h"

#define SQL_LENGTH 1024

int InsertSql(Local* l, MYSQL* m);

int RegistSelectSql(Local* l, MYSQL* m);

int LoginSelectSql(Local* l, MYSQL* m);


