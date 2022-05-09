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
    for (int i = 0; i < 50; ++i)
    {
        // int *num = new int(i + 100);
        pool.add_task(Task(task_func, make_shared<int>(i + 100)));
    }
    sleep(40);
    return 0;
}
