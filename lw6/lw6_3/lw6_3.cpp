#include <iostream>
#include <omp.h>
#include <chrono>

const int MATRIX_SIZE = 1000;

void multiplyMatrix(const double* A, const double* B, double* C, int size)
{
#pragma omp parallel for
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            double sum = 0;
            for (int k = 0; k < size; k++)
            {
                sum += A[i * size + k] * B[k * size + j];
            }
            C[i * size + j] = sum;
        }
    }
}

int main()
{
    double* A = new double[MATRIX_SIZE * MATRIX_SIZE];
    double* B = new double[MATRIX_SIZE * MATRIX_SIZE];
    double* C = new double[MATRIX_SIZE * MATRIX_SIZE];

    for (int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++)
    {
        A[i] = static_cast<double>(rand()) / RAND_MAX;
        B[i] = static_cast<double>(rand()) / RAND_MAX;
    }

    auto startTime = std::chrono::high_resolution_clock::now();

    multiplyMatrix(A, B, C, MATRIX_SIZE);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = endTime - startTime;

    std::cout << "Complete" << std::endl;
    std::cout << "Time = " << duration.count() << " sec" << std::endl;

    delete[] A;
    delete[] B;
    delete[] C;

    return 0;
}

//без parallel for time = 4.465 sec
//
//используя parallel for time = 0.664185 sec
//
//при распараллеливании библиотека создает несколько потоков и распределяет итерации цикла между ними
//каждый поток вычисляет свои строки матрицы C
//между потоками делится работа на равные доли -> время работы уменьшается, увеличивается производительность