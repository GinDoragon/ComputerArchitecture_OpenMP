#include <omp.h>
#include <iostream>
#include <windows.h>

// Функция для демонстрации параллельного цикла с директивой for
void parallel_for_example() {
#pragma omp parallel for
    for (int i = 0; i < 100; i++) {
        int thread_id = omp_get_thread_num();
        std::cout << "Thread " << thread_id << " is executing iteration " << i << "\n";
    }
}

// Функция для демонстрации использования директив single и critical
void single_and_critical_example() {
#pragma omp parallel
    {
        int thread_id = omp_get_thread_num();

#pragma omp single
        {
            Sleep(150);
            std::cout << "This will be printed by only one thread: " << thread_id << "\n";
        }

#pragma omp critical
        {
            Sleep(15);
            std::cout << "Thread " << thread_id << " is in critical section.\n";
        }
    }
}

// Функция для демонстрации использования замков
void lock_example() {
    omp_lock_t lock;
    omp_init_lock(&lock);

#pragma omp parallel
    {
        int thread_id = omp_get_thread_num();

        omp_set_lock(&lock);
        std::cout << "Thread " << thread_id << " is in critical section with lock.\n";
        omp_unset_lock(&lock);
    }

    omp_destroy_lock(&lock);
}

int main() {
    omp_set_num_threads(10);

    std::cout << "Running parallel for example:\n";
    parallel_for_example();

    std::cout << "\nRunning single and critical example:\n";
    single_and_critical_example();

    std::cout << "\nRunning lock example:\n";
    lock_example();

    return 0;
}
