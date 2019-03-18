#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define MAX_THREAD_LIMIT 10 //最大上限
#define THREAD_INIT 5 // 初始化个数

typedef struct WorkReadyQueue {
  void* (*WorkFunction)(void* arg); // 函数指针
  void* arg; // 参数
  struct WorkReadyQueue* next;
} WorkReadyQueue; // 任务就绪队列

typedef struct ThreaddPool {
  pthread_mutex_t queue_lock; 
  pthread_cond_t queue_ready; 
  pthread_mutex_t thread_count_lock;

  int thread_work_count;

  WorkReadyQueue* queue_head; // 任务队列头指针
  int work_queue_size; // 任务个数

  pthread_t* thread_id; // 线程 id 数组

  int thread_init; // 初始化线程数
  int max_thread_limit; // 线程池最大上限
  int thread_num; // 线程池内实际线程数

  int destroy_flag; // 防止重复销毁
} ThreaddPool; // 线程池内元素

void ThreadPoolInit(ThreaddPool* tp); // 初始化线程池

void AddWorkQueue(ThreaddPool* tp, void* (*WorkFunction)(void* arg), void* arg); // 给任务就绪队列添加任务

void ThreaddPoolDestroy(ThreaddPool* tp); // 销毁线程池

void* DefaultWork(void* arg); // 默认任务

void DilationThread(ThreaddPool* tp); // 扩容
