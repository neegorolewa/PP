#include "omp.h"
#include <iostream>
#include <chrono>

constexpr long NUM_ITERATION = 1000000000;

//for синхронно
double getPiSync(long numIt)
{
	double pi = 0;

	for (long i = 0; i < numIt; i++)
	{
		if (i % 2 == 0)
		{
			pi += 1.0 / (2 * i + 1);
		}
		else
		{
			pi -= 1.0 / (2 * i + 1);
		}
	}

	return pi * 4.0;

}

//параллельный for (неверно)
double getPiParallelWrong(long numIt)
{
	double pi = 0;

#pragma omp parallel for default(none) shared(pi) shared(numIt)
	for (long i = 0; i < numIt; i++)
	{
		if (i % 2 == 0)
		{
			pi += 1.0 / (2 * i + 1);
		}
		else
		{
			pi -= 1.0 / (2 * i + 1);
		}

	}

	return pi * 4.0;
}

//parallel for и atomic (рабочее)
double getPiParallelAtomic(long numIt)
{
	double pi = 0;

#pragma omp parallel for default(none) shared(pi, numIt) 
	for (long i = 0; i < numIt; i++)
	{
		if (i % 2 == 0)
		{
#pragma omp atomic
			pi += 1.0 / (2 * i + 1);
		}
		else
		{
#pragma omp atomic
			pi -= 1.0 / (2 * i + 1);
		}

	}

	return pi * 4.0;
}

//reduction (рабочее)
double getPiParallelReduction(long numIt)
{
	double pi = 0;

#pragma omp parallel for reduction(+:pi)
	for (long i = 0; i < numIt; i++)
	{
		if (i % 2 == 0)
		{
			pi += 1.0 / (2 * i + 1);
		}
		else
		{
			pi -= 1.0 / (2 * i + 1);
		}

	}

	return pi * 4.0;
}

int main()
{
	//for (синхронно) 
	//Value of PI (sync): 3.14159 Time: 3.23895 sec
	auto startTime = std::chrono::high_resolution_clock::now();
	double pi = getPiSync(NUM_ITERATION);
	auto endTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = endTime - startTime;
	std::cout << "Value of PI (sync): " << pi << " Time: " << duration.count() << " sec" << std::endl;

	//параллельный for (неверно) 
	//Value of PI (parallel wrong): -0.000238529 Time: 9.54797 sec
	startTime = std::chrono::high_resolution_clock::now();
	pi = getPiParallelWrong(NUM_ITERATION);
	endTime = std::chrono::high_resolution_clock::now();
	duration = endTime - startTime;
	std::cout << "Value of PI (parallel wrong): " << pi << " Time: " << duration.count() << " sec" << std::endl;

	//параллельный for и atomic (рабочее)
	//медленно из-за синхронизации
	//Value of PI (parallel atomic): 3.14159 Time: 101.573 sec
	startTime = std::chrono::high_resolution_clock::now();
	pi = getPiParallelAtomic(NUM_ITERATION);
	endTime = std::chrono::high_resolution_clock::now();
	duration = endTime - startTime;
	std::cout << "Value of PI (parallel atomic): " << pi << " Time: " << duration.count() << " sec" << std::endl;

	//reduction (рабочее) 
	//Value of PI (reduction): 3.14159 Time: 0.329075 sec
	startTime = std::chrono::high_resolution_clock::now();
	pi = getPiParallelReduction(NUM_ITERATION);
	endTime = std::chrono::high_resolution_clock::now();
	duration = endTime - startTime;
	std::cout << "Value of PI (reduction): " << pi << " Time: " << duration.count() << " sec" << std::endl;

}