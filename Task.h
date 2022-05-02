#ifndef __TASK__
#define __TASK__
using callback = void(*)(void*);
struct Task {
    Task() : function(nullptr), arg(nullptr) {};
    Task(callback f, void* arg) : function(f), arg(arg) {};
    ~Task() = default;

    callback function;
    void* arg;
};
#endif
