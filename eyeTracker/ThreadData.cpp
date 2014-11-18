#include "ThreadData.h"


ThreadData::ThreadData(void) {StartThread();}
ThreadData::~ThreadData(void){}

//starts a thread by setting _running to true
bool ThreadData::StartThread()
{
	_running = true;
	// Start CLEye image capture thread
	_hThread = CreateThread(NULL, 0, &ThreadData::DataThread, this, 0, &_threadId);
	if(_hThread == NULL)
	{
		return false;
	}
	return true;
}

unsigned long __stdcall ThreadData::DataThread(void* instance)
{
	// seed the rng with current tick count and thread id
	srand(GetTickCount() + GetCurrentThreadId());
	// forward thread to Capture function
	ThreadData *pThis = (ThreadData *)instance;
	pThis->Run();
	return 0;
}

//main method of the ThreadData, this is where the stuff happens
void ThreadData::Run()
{
	MSG msg;
	//wait for receiving message
	while (::GetMessage( &msg, NULL, 0, 0 ))
		{
 cout << (char*)msg.wParam; //Display what was received in wParam of the message<br/>
 }
	/*
	while(_running)
	{
		cout << "ThreadID: " << GetCurrentThreadId() << endl;
	}
	*/
}

// stops a running thread
void ThreadData::StopThread()
{
	if(!_running)   return;
	_running = false;
	WaitForSingleObject(_hThread, 1000);
}
//returns the thread-ID
DWORD ThreadData::getThreadId()
{
	return _threadId;
}