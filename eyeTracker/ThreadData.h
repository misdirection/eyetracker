#pragma once
#include <iostream>
#include "eyeTracker.h"
#include "Thread.h"

class ThreadData : public Thread
{
public:
	ThreadData(void);
	~ThreadData(void);
	void Run();

private:
};

