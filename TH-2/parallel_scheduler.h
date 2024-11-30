#pragma once

#include <pthread.h>
#include <queue>
#include <functional>

class parallel_scheduler {
public:
    parallel_scheduler(size_t size);
    ~parallel_scheduler();

    void run(std::function<void(void*)> func, void* arg);

private:
    struct task {
        std::function<void(void*)> function;
        void* arg;
    };

    void work_loop();
    static void* work_thread(void* arg);

    size_t size;
    pthread_t* threads;
    std::queue<task> tasks;
    pthread_mutex_t queue_mutex;
    pthread_cond_t queue_cond;
    bool terminate_flag;
};
