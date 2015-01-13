#pragma once
#include "eyeTracker.h"

class Thread
{
public:
	Thread(void);
	~Thread(void);
	bool Start();
	static unsigned long __stdcall callThread(void*);
	virtual inline void Run() {};
	void Stop();
	void ThreadCom_send(DWORD, WPARAM);
	void ThreadCom_reply(MSG);
	DWORD getThreadId();
	bool isRunning();
	struct dataPackage{vector<Point> pupilPos;bool firstpackage;int id;} *data;

protected:
	bool _running;
	HANDLE _hThread;
	DWORD _threadId;

private: 
};

