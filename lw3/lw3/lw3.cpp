#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>
#include <strsafe.h>
#include <tchar.h>
#include <windows.h>
#include <chrono>

const int COUNT_OPERATION = 20;

std::chrono::steady_clock::time_point startTime;

DWORD WINAPI ThreadProc(const LPVOID lpParam)
{
	int thIndex = *(static_cast<int*>(lpParam));
	std::ofstream out1("out1.txt", std::ios_base::app);
	std::ofstream out2("out2.txt", std::ios_base::app);


	if (!out1.is_open() || !out2.is_open())
	{
		std::cerr << "Failed to open files for writing." << std::endl;
		ExitThread(1);
	}

	for (int i = 0; i < COUNT_OPERATION; i++)
	{
		Sleep(100);
		auto currentTime = std::chrono::steady_clock::now();
		auto time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
		
		if (thIndex == 1)
		{
			out1 << thIndex << "|" << time << std::endl;
		}

		if (thIndex == 2)
		{
			out2 << thIndex << "|" << time << std::endl;

		}
	}

	out1.close();
	out2.close();

	ExitThread(0);
}

int main(int argc, char* argv[])
{
	int numberThreads = 2;

	HANDLE* handles = new HANDLE[numberThreads];
	int* threadIndices = new int[numberThreads];
	std::cin.get();

	startTime = std::chrono::steady_clock::now();

	for (int i = 0; i < numberThreads; i++)
	{
		threadIndices[i] = i + 1;

		handles[i] = CreateThread(NULL, 0, &ThreadProc, &threadIndices[i], CREATE_SUSPENDED, NULL);

		if (handles[i] == NULL)
		{
			std::cerr << "Error creating thread" << std::endl;
			return 1;
		}
	}

	for (int i = 0; i < numberThreads; i++)
	{
		ResumeThread(handles[i]);
	}

	WaitForMultipleObjects(numberThreads, handles, true, INFINITE);

	for (int i = 0; i < numberThreads; ++i)
	{
		CloseHandle(handles[i]);
	}

	delete[] handles;
	delete[] threadIndices;

	return 0;
}