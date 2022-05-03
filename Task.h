#ifndef __TASK__
#define __TASK__
#include <memory>
using callback = void(*)(std::shared_ptr<int>);
struct Task {
    Task() : function(nullptr), arg(nullptr) {};
    Task(callback f, std::shared_ptr<int> arg) : function(f), arg(arg) {};
    ~Task() = default;

    callback function;
    std::shared_ptr<int> arg;
};
#endif
