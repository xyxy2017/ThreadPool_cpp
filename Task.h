#ifndef __TASK__
#define __TASK__
using callback = void(*)(int*);
struct Task {
    Task() : function(nullptr), arg(nullptr) {};
    Task(callback f, int* arg) : function(f), arg(arg) {};
    ~Task() = default;

    callback function;
    int* arg;
};
#endif
