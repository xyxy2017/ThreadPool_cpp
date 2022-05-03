#include "ThreadPool.h"
#include <iostream>
#include <memory>
using namespace std;

void task_func(shared_ptr<int> arg)
{
    int num = *arg;
    printf("thread:%ld is working, number = %d\n", pthread_self(), num);
    sleep(1);
}

int main()
{
    // 创建线程池
    ThreadPool pool(3, 10);
    for (int i = 0; i < 20; ++i)
    {
        // int *num = new int(i + 100);
        pool.add_task(Task(task_func, make_shared<int>(i + 100)));
    }
    sleep(20);
    return 0;
}
