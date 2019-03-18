.PHONY:all
all:client server

client:client.c comm.c  game.c
        gcc -g -o $@ $^ -lpthread
server:server.c comm.c  game.c game_home.c person_info.c game_info.c thread_pool.c mysql_operate.c
        gcc -g -o  $@ $^ -lpthread  -I/usr/include/mysql -L/usr/lib/mysql  -L/usr/lib64/mysql -lmysqlclient
.PHONY:clean

clean:
        rm -f server client

