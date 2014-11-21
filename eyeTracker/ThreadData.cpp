#include "ThreadData.h"

ThreadData::ThreadData(void) {Thread();Start();}
ThreadData::~ThreadData(void){}


//main method of the ThreadData, this is where the stuff happens
void ThreadData::Run()
{
	MSG msg;
	//wait for receiving message
	while (::GetMessage( &msg, NULL, 0, 0 ))
		{
			// get data package from other thread
			data = (dataPackage*)msg.wParam;
			cout << data->x;
			// set event to tell other thread to go on
	
			ThreadCom_reply(msg);
			//			if ((char*)msg.lParam == "threadId") {cout << (unsigned long)msg.wParam;} //Display what was received in wParam of the message<br/>
 }
}
