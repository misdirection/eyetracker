#include "Thread.h"

Thread::Thread(void):
	_running(false)
{
}


Thread::~Thread(void)
{
}


void Thread::ThreadCom_send(DWORD threadId, WPARAM param)
{
  HANDLE h = CreateEvent(NULL, 0, 0, NULL);
  ::PostThreadMessage(threadId,(WM_APP + 3), param, (LPARAM)h);
  WaitForSingleObject(h, INFINITE);
  CloseHandle(h);
}

void Thread::ThreadCom_reply(MSG msg)
{
  SetEvent((HANDLE)msg.lParam);
}



// stops the capturing
void Thread::Stop()
{
	if(!_running)   return;
	_running = false;
	WaitForSingleObject(_hThread, 1000);
}

// return the ID of a thread
DWORD Thread::getThreadId()
{
	return _threadId;
}


unsigned long __stdcall Thread::callThread(void* instance)
{
	// forward thread to Capture function
	Thread *pThis = (Thread *)instance;
	pThis->Run();
	return 0;
}

//starts a  Capturing by setting _running to true
bool Thread::Start()
{
	_running = true;
	// Start CLEye image capture thread
	_hThread = CreateThread(NULL, 0, &Thread::callThread, this, 0, &_threadId);
	if(_hThread == NULL)
	{
		return false;
	}
	return true;
}

bool Thread::isRunning()
{
	return _running;
}
