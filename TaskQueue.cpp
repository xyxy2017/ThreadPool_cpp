#include "TaskQueue.h"

TaskQueue::TaskQueue() {
    pthread_mutex_init(&m_mutex, NULL);
}
TaskQueue::~TaskQueue() {
    pthread_mutex_destroy(&m_mutex);
}

void TaskQueue::add_task(Task &t) {
    pthread_mutex_lock(&m_mutex);
    m_queue.push(t);
    pthread_mutex_unlock(&m_mutex);
}

void TaskQueue::add_task(callback f, void* arg) {
    pthread_mutex_lock(&m_mutex);
    m_queue.push(Task(f, arg));
    // m_queue.push(f, arg);
    pthread_mutex_unlock(&m_mutex);
}

Task TaskQueue::take_task() {
    Task t;
    pthread_mutex_lock(&m_mutex);
    if (m_queue.size() > 0) {
        t = m_queue.front();
        m_queue.pop();
    }
    pthread_mutex_unlock(&m_mutex);
    return t;
}