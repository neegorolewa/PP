#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <string>
#include <iostream>
#include <cstddef>

DWORD WINAPI ThreadProc(const LPVOID lpParam)
{
	int thIndex = *(static_cast<int*>(lpParam));
	std::cout << "Thread " << thIndex << " does his job\n";
	ExitThread(0);
}

int _tmain(int argc, _TCHAR* argv[])
{
	int numTh;

	std::cout << "Enter the number of threads: ";
	std::cin >> numTh;

	HANDLE* handles = new HANDLE[numTh];
	int* threadIndices = new int[numTh];

	for (int i = 0; i < numTh; i++)
	{
		threadIndices[i] = i;
		
		handles[i] = CreateThread(NULL, 0, &ThreadProc, &threadIndices[i], CREATE_SUSPENDED, NULL);

		if (handles[i] == NULL)
		{
			std::cout << "Error thread" << std::endl;
			return 0;
		}
	}

	for (int i = 0; i < numTh; i++)
	{
		ResumeThread(handles[i]);
		Sleep(1000);
	}

	WaitForMultipleObjects(numTh, handles, true, INFINITE);

	return 0;
}