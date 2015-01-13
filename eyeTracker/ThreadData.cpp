#include "ThreadData.h"

ThreadData::ThreadData(void) {Thread();Start();}
ThreadData::~ThreadData(void){}



//main method of the ThreadData, this is where the stuff happens
void ThreadData::Run()
{
	// weißer hintergrund

	int dim1=800,dim2=600;
	namedWindow("output", CV_WINDOW_NORMAL);
	setWindowProperty("output", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
	Mat base(dim1,dim2, CV_32F);
//	imshow("output",base);
//	waitKey(1000);
	
	MSG msg;
	calibprocess=-1;
	int counter=0;
	//wait for receiving message
	while (::GetMessage( &msg, NULL, 0, 0 ))
		{
	imshow("output",base);
	waitKey(1000);

			data = (dataPackage*)msg.wParam;
			// set all receivedThreads
			if (data->firstpackage == true) {receivedThreads.push_back(data->id);}
			if (calibprocess<9 && counter >20)
			{
				//calib_tmp[calibCircle][left,right][threadNumber][vectorOfPoints]
				if (data->pupilPos[0]!=Point(0,0) && calib_tmp[calibprocess][0][data->id].size()<=30) {calib_tmp[calibprocess][0][data->id].push_back(data->pupilPos[0]);}
				if (data->pupilPos[1]!=Point(0,0) && calib_tmp[calibprocess][1][data->id].size()<=30) {calib_tmp[calibprocess][1][data->id].push_back(data->pupilPos[1]);}
				int nextStep=1;
				for (int i=0;i<receivedThreads.size();i++)
				{if (calib_tmp[calibprocess][0][i].size()<30 || calib_tmp[calibprocess][1][i].size()<30) {nextStep=0;}}
				if (nextStep==1) {calibprocess++;counter=0;}
			}
			// get data package from other thread
			//cout << data->x;
			// set event to tell other thread to go on
	
			ThreadCom_reply(msg);
			//			if ((char*)msg.lParam == "threadId") {cout << (unsigned long)msg.wParam;} //Display what was received in wParam of the message<br/>
		
		counter++;
		}
}
