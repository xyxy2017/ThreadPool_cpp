#include "ThreadPool.h"

ThreadPool::ThreadPool(int min, int max) : m_minNum(min), m_maxNum(max) {
    // ʵ�����������
    m_taskQ = new TaskQueue();

    do {
        // ��ʼ���̳߳�
        m_busyNum = 0;
        m_exitNum = 0;
        m_liveNum = m_minNum;
    
        // �����߳������ڴ�
        m_threadIDs = new pthread_t[m_maxNum];
        if (m_threadIDs == nullptr) {
            std::cout << "new m_threadIDs failed..." << std::endl;
            break; 
        }
        memset(m_threadIDs, 0, m_maxNum * sizeof(pthread_t));
        // ��ʼ���߳�������������
        if (pthread_mutex_init(&m_lock, NULL) != 0 || 
            pthread_cond_init(&m_notEmpty, NULL) != 0) {
            std::cout << "init mutex or cond failed..." << std::endl;
            break;
        }

        // �����߳�
        // ����С���������߳�
        for (int i = 0; i < m_minNum; ++i) {
            pthread_create(&m_threadIDs[i], NULL, worker, this);
            std::cout << "create thread, ID:" << m_threadIDs[i] << std::endl;
        }
        pthread_create(&m_managerID, NULL, manager, this);
    } while (0);

}

ThreadPool::~ThreadPool() {
    m_shutdown = true;
    pthread_join(m_managerID, NULL);
    for (int i = 0; i < m_liveNum; ++i) {
        std::cout << "the thread poll is closing, the live thread is exiting..." << std::endl;
        pthread_cond_signal(&m_notEmpty);
    }
    sleep(1);   // for live thread exit

    if (m_taskQ)    delete m_taskQ;
    if (m_threadIDs)    delete[] m_threadIDs;
    pthread_cond_destroy(&m_notEmpty);
    pthread_mutex_destroy(&m_lock);
}

void ThreadPool::add_task(Task t) {
    if (m_shutdown) return;
    // ���task
    m_taskQ->add_task(t);
    // �����������߳�
    pthread_cond_signal(&m_notEmpty);
}

void* ThreadPool::worker(void* arg) {
    ThreadPool* pool = static_cast<ThreadPool*>(arg);
    while (1) {
        // �����̳߳���Դ������
        pthread_mutex_lock(&pool->m_lock);
        // �ж���������Ƿ�Ϊ�գ����Ƿ��̳߳عر�
        while (!pool->m_shutdown && pool->m_taskQ->task_number() <= 0) {
            std::cout << "thread:" << pthread_self() << " waiting..." << std::endl;
            // �����߳� 
            pthread_cond_wait(&pool->m_notEmpty, &pool->m_lock);
            if (pool->m_exitNum > 0) {
                pool->m_exitNum--;
                if (pool->m_liveNum > pool->m_minNum) {
                    pool->m_liveNum--;
                    pthread_mutex_unlock(&pool->m_lock);
                    pool->thread_exit();
                }
            }
        }
        if (pool->m_shutdown) {
            pool->m_liveNum--;
            pthread_mutex_unlock(&pool->m_lock);
            pool->thread_exit();
        }
        // ȡ����
        Task t = pool->m_taskQ->take_task();
        pool->m_busyNum++;
        pthread_mutex_unlock(&pool->m_lock);
        
        // ִ������
        std::cout << "thread:" << pthread_self() << " is working..." << std::endl;
        t.function(t.arg);
        delete (int*)t.arg;
        t.arg = nullptr;
        // ִ�����
        std::cout << "thread:" << pthread_self() << " end working..." << std::endl;
        pthread_mutex_lock(&pool->m_lock);
        pool->m_busyNum--;
        pthread_mutex_unlock(&pool->m_lock);
    }
    return nullptr;
}

void* ThreadPool::manager(void* arg) {
    ThreadPool* pool = static_cast<ThreadPool*>(arg);
    while (!pool->m_shutdown) {
        sleep(5);
        pthread_mutex_lock(&pool->m_lock);
        int queue_size = pool->m_taskQ->task_number();
        int live_num = pool->m_liveNum;
        int busy_num = pool->m_busyNum;
        pthread_mutex_unlock(&pool->m_lock);

        // һ�δ���/�ͷ��߳�
        const int NUMBER = 2;
        // ����࣬�����߳�
        if (queue_size > live_num && live_num < pool->m_maxNum) {
            pthread_mutex_lock(&pool->m_lock);
            int num = 0;
            for (int i = 0; i < pool->m_maxNum && num < NUMBER && live_num < pool->m_maxNum; ++i) {
                if (pool->m_threadIDs[i] == 0) {
                    pthread_create(&pool->m_threadIDs[i], NULL, worker, pool);
                    ++num;
                    ++pool->m_liveNum;
                    std::cout << "manager: add a live thread, the live thread is " << pool->m_liveNum << std::endl;
                }
            }
            pthread_mutex_unlock(&pool->m_lock);
        }

        // �����̶߳࣬�ͷ��߳�
        if (live_num > busy_num * 2 && live_num > pool->m_minNum) {
            pthread_mutex_lock(&pool->m_lock);
            pool->m_exitNum = NUMBER;
            pthread_mutex_unlock(&pool->m_lock);
            for (int i = 0; i < NUMBER; ++i) {
                std::cout << "the free thread exiting..." << std::endl;
                pthread_cond_signal(&pool->m_notEmpty);
            }
        }
    }
    return nullptr;
}

void ThreadPool::thread_exit() {
    pthread_t tid = pthread_self();
    for (int i = 0; i < m_maxNum; ++i) {
        if (m_threadIDs[i] == tid) {
            std::cout << "thread:" << tid << " exiting... leaving " << m_liveNum << " threads" << std::endl;
            m_threadIDs[i] = 0;
            break;
        }
    }
    pthread_exit(NULL);
}