#include "omp.h"
#include <iostream>

int main()
{
    int x = 44;
//#pragma omp parallel for private(x)
//#pragma omp parallel for firstprivate(x)
//#pragma omp parallel for lastprivate(x)
    for (int i = 0; i <= 10; i++) {
        x += i;
        printf("Thread number: %d x: %d\n", omp_get_thread_num(), x);
    }
    printf("x is %d\n", x);
}

//private(x) - каждый поток получает частную свою копию переменной X, не копируя ее начальное значение
//по окончании частные значения не переносятся в исходную X

//firstprivate(x) - каждый поток получает частную свою копию переменной X, копируя ее начальное значение
//по окончании частные значения не переносятся в исходную X

//lastprivate(x) - каждый поток получает частную свою копию переменной X, не копируя ее начальное значение
//по окончании X принимает значение последней итерации