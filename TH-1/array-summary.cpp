#include <iostream>
#include <pthread.h>
#include <chrono>

long long* partial_sums;
int* arr;
size_t N, M;

void* partial_sum(void* arg) {
    size_t thread_id = *(size_t*)arg;
    size_t start = (N / M) * thread_id;
    size_t end = (thread_id == M - 1) ? N : start + (N / M);

    long long sum = 0;
    for (size_t i = start; i < end; ++i) {
        sum += arr[i];
    }
    partial_sums[thread_id] = sum;
    return nullptr;
}

int main(int argc, char** argv) {
    N = std::stoull(argv[1]);
    M = std::stoull(argv[2]);

    arr = new int[N];
    for (size_t i = 0; i < N; ++i) {
        arr[i] = (i % 100) + 1;
    }

    auto start_time = std::chrono::high_resolution_clock::now();
    long long single_thread_sum = 0;
    for (size_t i = 0; i < N; ++i) {
        single_thread_sum += arr[i];
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto single_thread_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    partial_sums = new long long[M];
    pthread_t* threads = new pthread_t[M];
    size_t* thread_ids = new size_t[M];

    start_time = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < M; ++i) {
        thread_ids[i] = i;
        pthread_create(&threads[i], nullptr, partial_sum, &thread_ids[i]);
    }

    for (size_t i = 0; i < M; ++i) {
        pthread_join(threads[i], nullptr);
    }

    long long multi_thread_sum = 0;
    for (size_t i = 0; i < M; ++i) {
        multi_thread_sum += partial_sums[i];
    }
    end_time = std::chrono::high_resolution_clock::now();
    auto multi_thread_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "Single-threaded sum: " << single_thread_sum << "\n";
    std::cout << "Multi-threaded sum: " << multi_thread_sum << "\n";
    std::cout << "Time spent without threads: " << single_thread_duration.count() << " ms\n";
    std::cout << "Time spent with " << M << " threads: " << multi_thread_duration.count() << " ms\n";

    delete[] arr;
    delete[] partial_sums;
    delete[] threads;
    delete[] thread_ids;

    return 0;
}

