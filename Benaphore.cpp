#include "Benaphore.h"
#include <windows.h>


Benaphore::Benaphore(int count)
{
	if (count < 1) count = 10;
	fSemaphore = CreateSemaphore(NULL, 0, count, NULL);
	fFlag = 0;
}

Benaphore::~Benaphore()
{
	CloseHandle((HANDLE)fSemaphore);
}

void Benaphore::Lock(void)
{
	//if (atomic_add(&flag, 1) != 1)
	if (InterlockedExchangeAdd(&fFlag, 1) > 0)
	{
		WaitForSingleObject((HANDLE)fSemaphore, INFINITE);
	}
}

void Benaphore::Unlock(void)
{
	//if (atomic_add(&flag, -1) != 0)
	if (InterlockedExchangeAdd(&fFlag, -1) > 0)
	{
		ReleaseSemaphore((HANDLE)fSemaphore, 1, NULL);
	}
}