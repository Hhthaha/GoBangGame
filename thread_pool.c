#include "thread_pool.h"

void* DefaultWork(void* arg) // 默认任务
{
  ThreaddPool* tp = (ThreaddPool*)arg;

  while (1) {
    pthread_mutex_lock(&(tp->queue_lock));

    while (tp->work_queue_size == 0 && tp->destroy_flag == 0) { // 线程池不销毁，且没有任务
     // printf("thread 0x%lu is waiting \n", pthread_self());
      pthread_cond_wait(&(tp->queue_ready), &(tp->queue_lock)); // 在此阻塞
    }
    
    if (tp->destroy_flag == 1) {
      pthread_mutex_unlock(&(tp->queue_lock));
      pthread_exit(0);
    }

    WorkReadyQueue* tmp = tp->queue_head;
    tmp->next = NULL;
    tp->queue_head = tp->queue_head->next;
    tp->work_queue_size--;

    pthread_mutex_unlock(&(tp->queue_lock));

    pthread_mutex_lock(&(tp->thread_count_lock));
    tp->thread_work_count++;
    (*(tmp->WorkFunction))(tmp->arg);
    pthread_mutex_unlock(&(tp->thread_count_lock));

    free(tmp);
    tmp = NULL;
  }

  return NULL;
}

void ThreadPoolInit(ThreaddPool* tp) // 初始化线程池
{
  pthread_mutex_init(&(tp->queue_lock), NULL);
  pthread_mutex_init(&(tp->thread_count_lock), NULL);
  pthread_cond_init(&(tp->queue_ready), NULL);

  tp->thread_work_count = 0;

  tp->queue_head = NULL;
  tp->work_queue_size = 0;

  tp->thread_init = THREAD_INIT;
  tp->max_thread_limit = MAX_THREAD_LIMIT;
  tp->thread_num = THREAD_INIT;

  tp->thread_id = (pthread_t*)malloc(sizeof(pthread_t) * tp->max_thread_limit);
  int i = 0;
  for (; i < tp->thread_init; ++i) {
    pthread_create(&(tp->thread_id[i]), NULL, DefaultWork, (void*)tp);
  }

  tp->destroy_flag = 0;

  return;
}

void AddWorkQueue(ThreaddPool* tp, void* (*WorkFunction)(void* arg), void* arg) // 给任务就绪队列添加任务
{
  if (tp == NULL) {
    return;
  }

  WorkReadyQueue* tmp = (WorkReadyQueue*)malloc(sizeof(WorkReadyQueue));
  tmp->arg = arg;
  tmp->next = NULL;
  tmp->WorkFunction = WorkFunction;

  pthread_mutex_lock(&(tp->queue_lock));

  if (tp->queue_head == NULL) {
    tp->queue_head = tmp;
  } else {
    WorkReadyQueue* cur = tp->queue_head;
    while (cur != NULL) {
      cur = cur->next;
    }
    cur = tmp;
  }
  tp->work_queue_size++;

  pthread_mutex_unlock(&(tp->queue_lock));

  // 任务添加完毕后，唤醒线程池内一个线程
  pthread_cond_signal(&(tp->queue_ready));

  return;
}

void ThreaddPoolDestroy(ThreaddPool* tp) // 销毁线程池
{
  if (tp == NULL) {
    return;
  }
  if (tp->destroy_flag == 1) { // 防止重复销毁
    return;
  }
  tp->destroy_flag = 1;

  pthread_mutex_destroy(&(tp->queue_lock));
  pthread_mutex_destroy(&(tp->thread_count_lock));
  pthread_cond_destroy(&(tp->queue_ready));

  WorkReadyQueue* cur = tp->queue_head;
  while (cur != NULL) {
    WorkReadyQueue* tmp = cur;
    cur = cur->next;
    free(tmp);
  }
  tp->queue_head = NULL;

  int i = 0;
  for (; i < tp->thread_num; ++i) {
    pthread_join(tp->thread_id[i], NULL);
  }
  free(tp->thread_id);
  tp->thread_id = NULL;

  tp->max_thread_limit = 0;
  tp->thread_num = 0;
  tp->thread_init = 0;
  tp->work_queue_size = 0;

  return;
}

void DilationThread(ThreaddPool* tp) // 扩容
{
  if (tp == NULL) {
    return;
  }

  if (tp->thread_work_count < tp->thread_num) {
    return;
  }

  if (tp->thread_num <= tp->max_thread_limit) {
    return;
  }

  int i = tp->thread_num;
  pthread_mutex_lock(&(tp->thread_count_lock));
  tp->thread_num++;
  tp->thread_work_count++;
  pthread_mutex_unlock(&(tp->thread_count_lock));
  pthread_create(&(tp->thread_id[i]), NULL, DefaultWork, (void*)tp);

  return;
}


