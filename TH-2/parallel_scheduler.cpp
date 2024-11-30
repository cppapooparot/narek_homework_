#include "parallel_scheduler.h"

parallel_scheduler::parallel_scheduler(size_t size) : size(size), terminate_flag(false) {
    threads = new pthread_t[size];
    pthread_mutex_init(&queue_mutex, nullptr);
    pthread_cond_init(&queue_cond, nullptr);

    for (size_t i = 0; i < size; i++) {
        pthread_create(&threads[i], nullptr, work_thread, this);
    }
}

parallel_scheduler::~parallel_scheduler() {
    pthread_mutex_lock(&queue_mutex);
    terminate_flag = true;
    pthread_cond_broadcast(&queue_cond);
    pthread_mutex_unlock(&queue_mutex);

    for (size_t i = 0; i < size; i++) {
        pthread_join(threads[i], nullptr);
    }

    pthread_mutex_destroy(&queue_mutex);
    pthread_cond_destroy(&queue_cond);
    delete[] threads;
}

void parallel_scheduler::run(std::function<void(void*)> func, void* arg) {
    pthread_mutex_lock(&queue_mutex);
    tasks.push({func, arg});
    pthread_cond_signal(&queue_cond);
    pthread_mutex_unlock(&queue_mutex);
}

void* parallel_scheduler::work_thread(void* arg) {
    auto* scheduler = static_cast<parallel_scheduler*>(arg);
    scheduler->work_loop();
    return nullptr;
}

void parallel_scheduler::work_loop() {
    while (true) {
        task task;

        pthread_mutex_lock(&queue_mutex);

        while (tasks.empty() && !terminate_flag) {
            pthread_cond_wait(&queue_cond, &queue_mutex);
        }

        if (terminate_flag && tasks.empty()) {
            pthread_mutex_unlock(&queue_mutex);
            break;
        }

        task = tasks.front();
        tasks.pop();

        pthread_mutex_unlock(&queue_mutex);

        task.function(task.arg);
    }
}

