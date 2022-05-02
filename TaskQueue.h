#ifndef __TASKQUEUE__
#define __TASKQUEUE__
#include <queue>
#include "Task.h"
#include <pthread.h>

class TaskQueue {
public:
    TaskQueue();
    ~TaskQueue();

    // add task to task m_queue
    void add_task(Task &t);
    void add_task(callback f, int* arg);
    
    // take a task from m_queue
    Task take_task();

    // get m_queue size
    int inline task_number() {
        return m_queue.size();
    }
private:
    pthread_mutex_t m_mutex;
    std::queue<Task> m_queue;
};
#endif