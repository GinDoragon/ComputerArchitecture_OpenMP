#include <iostream>
#include <cstdlib>
#include <ctime>
#include <omp.h>

using namespace std;

// Функция для выполнения вычислительной нагрузки
void func() {
    for (int i = 0; i < 50000; i++) {
        rand();
    }
}

int main() {
    setlocale(LC_ALL, "Russian");

    // Устанавливаем количество потоков равным 2
    omp_set_dynamic(0);
    omp_set_num_threads(10);

    cout << "Максимальное количество потоков (omp_get_max_threads): "
        << omp_get_max_threads() << "\n" << endl;

    // 1. Параллельный цикл с подтверждением числа потоков и использованием barrier
#pragma omp parallel
    {
#pragma omp single
        {
            cout << "Параллельная область (только один поток выполняет вывод), число потоков = "
                << omp_get_num_threads() << "\n" << endl;
        }

#pragma omp for
        for (int i = 0; i < 10; i++) {
#pragma omp critical
            {
                cout << "Параллельный for: поток " << omp_get_thread_num()
                    << " обрабатывает значение " << i << endl;
                func();
            }
        }
    }

    // 2. Параллельные секции
#pragma omp parallel
    {
#pragma omp single
        {
            cout << "Параллельная область (sections), число потоков = "
                << omp_get_num_threads() << "\n" << endl;
        }

#pragma omp sections
        {
#pragma omp section
            {
                for (int i = 0; i < 5; i++) {
#pragma omp critical
                    {
                        cout << "Секция 1: поток " << omp_get_thread_num() << endl;
                        func();
                    }
                }
            }

#pragma omp section
            {
                for (int i = 0; i < 5; i++) {
#pragma omp critical
                    {
                        cout << "Секция 2: поток " << omp_get_thread_num() << endl;
                        func();
                    }
                }
            }
        }
    }

#pragma omp barrier

#pragma omp master
    {
        cout << "Все потоки достигли barrier. (Выполнено потоком "
            << omp_get_thread_num() << ")\n" << endl;
    }

    // 3. Синхронизация потоков с использованием single и critical для заполнения массива
    const int arrSize = 10;
    int array[arrSize] = { 0 };

#pragma omp single
    {
        cout << "Инициализация массива выполняется потоком "
            << omp_get_thread_num() << "\n" << endl;
    }

    for (int i = 0; i < arrSize; i++) 
#pragma omp critical    
    {
        array[i] = omp_get_thread_num();
        cout << "Поток " << omp_get_thread_num() << " записал array[" << i << "]" << endl;
        func();
    }

    cout << "\nГотовый массив: ";
    for (int i = 0; i < arrSize; i++) {
        cout << array[i] << " ";
    }
    cout << "\n" << endl;

    // 4. Синхронизация на базе замков
    cout << "Демонстрация синхронизации с использованием замков:" << endl;

    omp_lock_t lock;
    omp_init_lock(&lock);

#pragma omp parallel
    {
        for (int i = 0; i < 5; i++) {
            omp_set_lock(&lock);
            cout << "Поток " << omp_get_thread_num() << " захватил замок (итерация " << i << ")" << endl;
            func();
            omp_unset_lock(&lock);
        }

        if (omp_test_lock(&lock)) {
            cout << "Поток " << omp_get_thread_num()
                << " успешно протестировал и захватил замок." << endl;
            omp_unset_lock(&lock);
        }
    }

    omp_destroy_lock(&lock);

    return 0;
}
