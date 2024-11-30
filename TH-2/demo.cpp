#include "parallel_scheduler.h"
#include <iostream>
#include <unistd.h>

void do_task(void* arg) {
    int id = *static_cast<int*>(arg);
    std::cout << "Task " << id << " is running now in thread with this id:" << pthread_self() << std::endl;
}

int main() {
    parallel_scheduler scheduler(3);

    int task_id[5];

    for (size_t i = 0; i < 5; i++) {
        task_id[i] = i + 1;
        scheduler.run(do_task, &task_id[i]);
    }

    sleep(5);
    return 0;
}

