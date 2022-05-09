#ifndef __THREADPOOL__
#define __THREADPOOL__
#include <pthread.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "TaskQueue.h"

class ThreadPool
{
public:
    ThreadPool(int min, int max);
    ~ThreadPool();
    
    void add_task(Task t);
    
private:
    static void* manager(void* arg);
    static void* worker(void* arg);
    void thread_exit();

private:
    TaskQueue *m_taskQ;

    int m_maxNum;
    int m_minNum;
    int m_liveNum;
    int m_exitNum;
    int m_busyNum;

    pthread_mutex_t m_lock;
    pthread_cond_t m_notEmpty;
    pthread_t *m_threadIDs;
    pthread_t m_managerID;
    bool m_shutdown = false;
};



#endif