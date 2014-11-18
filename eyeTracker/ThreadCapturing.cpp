#include "ThreadCapturing.h"
#include "fps.h"

ThreadCapturing::ThreadCapturing(void){};
ThreadCapturing::~ThreadCapturing(void){};
ThreadCapturing::ThreadCapturing(LPSTR windowName, VideoCapture captureDevice, DWORD dataThreadID) :
	_running(false), 
	_captureDevice(captureDevice),
	_dataThreadId(dataThreadID)
{
	strcpy_s(_windowName, windowName);
}

//starts a  Capturing by setting _running to true
bool ThreadCapturing::StartCapture()
{
	_running = true;
	// Start CLEye image capture thread
	_hThread = CreateThread(NULL, 0, &ThreadCapturing::CaptureThread, this, 0, &_threadId);
	if(_hThread == NULL)
	{
		return false;
	}
	return true;
}

unsigned long __stdcall ThreadCapturing::CaptureThread(void* instance)
{
	// forward thread to Capture function
	ThreadCapturing *pThis = (ThreadCapturing *)instance;
	pThis->Run();
	return 0;
}

// main method of the ThreadCapturing which runs after the StartCapture
void ThreadCapturing::Run()
{
	// test of  transfer to data thread only string here, but can be used with anything else
	char *p=new char;
	p="message from camera thread"; //Created on heap
	// send message of the thread to data thread
	PostThreadMessage(_dataThreadId,(WM_APP + 3),(WPARAM)p,0);

	fps framesPerSeconds;
	while(_running)
	{
		Mat frame;
		_captureDevice >> frame; // get a new frame from camera
		imshow(_windowName, frame); //displays an image in the specified window
		cout << "fps:" << framesPerSeconds.getFPS() << endl;
		if(waitKey(30) >= 0) break;
	}
}
// stops the capturing
void ThreadCapturing::StopCapture()
{
	if(!_running)   return;
	_running = false;
	WaitForSingleObject(_hThread, 1000);
}

// return the ID of a thread
DWORD ThreadCapturing::getThreadId()
{
	return _threadId;
}