#include "ThreadData.h"

ThreadData::ThreadData(void) {Thread();Start();}
ThreadData::~ThreadData(void){}


//main method of the ThreadData, this is where the stuff happens
void ThreadData::Run()
{
	namedWindow("output", CV_WINDOW_NORMAL);
	setWindowProperty("output", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
	Mat base(800,600, CV_32F);
	imshow("output",base);
	waitKey(1000);
	circle(base,Point(5,5),5,Scalar(255,0,0),CV_FILLED,3,0);
	imshow("output",base);
	waitKey(1000);
	circle(base,Point(300,400),5,Scalar(255,0,0),CV_FILLED,3,0);
	imshow("output",base);
	waitKey(1000);
	circle(base,Point(595,795),5,Scalar(255,0,0),CV_FILLED,3,0);
	imshow("output",base);
	waitKey(1000);

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
