#pragma once
#include <iostream>
#include "eyeTracker.h"


class ThreadData
{
public:
	ThreadData(void);
	~ThreadData(void);

	static unsigned long __stdcall DataThread(void*);
	bool StartThread();
	void StopThread();
	void Run();
	DWORD getThreadId();

private:
	HANDLE _hThread;
	bool _running;
	DWORD _threadId;
};

