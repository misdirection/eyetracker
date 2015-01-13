#include "ThreadData.h"

ThreadData::ThreadData(void) 
{
	dim1=640,dim2=360;
	namedWindow("output", CV_WINDOW_NORMAL);
	setWindowProperty("output", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
	Thread();
	Start();
	base=Mat(dim2,dim1, CV_32F);
	rectangle(base,Point(0,0),Point(dim1,dim2),Scalar(255,255,255),CV_FILLED);
	imshow("output",base);
}
ThreadData::~ThreadData(void){}

void ThreadData::drawCalibCircle(int cal)
{
	int x,y;
	if (cal==0)		 {x=dim1/100;		y=dim2/100;}
	else if (cal==1) {x=dim1/2;			y=dim2/100;}
	else if (cal==2) {x=dim1-(dim1/100);y=dim2/100;}
	else if (cal==3) {x=dim1/100;		y=dim2/2;}
	else if (cal==4) {x=dim1/2;			y=dim2/2;}
	else if (cal==5) {x=dim1-(dim1/100);y=dim2/2;}
	else if (cal==6) {x=dim1/100;		y=dim2-(dim2/100);}
	else if (cal==7) {x=dim1/2;			y=dim2-(dim2/100);}
	else if (cal==8) {x=dim1-(dim1/100);y=dim2-(dim2/100);}

	else {x=dim1-(dim1/100);	y=dim1-(dim1/100);}
	circle(base,Point(x,y),dim2/100,Scalar(0,0,0),CV_FILLED);
}


//main method of the ThreadData, this is where the stuff happens
void ThreadData::Run()
{
	MSG msg;
	calibprocess=-1;
	int counter=0;
	//wait for receiving message
	while (::GetMessage( &msg, NULL, 0, 0 ))
		{
			//base=Mat(dim2,dim1, CV_32F);
			//white background
			rectangle(base,Point(0,0),Point(dim1,dim2),Scalar(255,255,255),CV_FILLED);
			data = (dataPackage*)msg.wParam;
			// set all receivedThreads
			if (data->firstpackage == true) {receivedThreads.push_back(data->id);}
			//first waitingtime and ignore first packages
			if (calibprocess==-1 && counter>5) {calibprocess++;counter=0;}
			if (calibprocess>-1 && calibprocess<9)
			{
				drawCalibCircle(calibprocess);
			}
			if (calibprocess>-1 && calibprocess<9 && counter >10)
			{
				//calib_tmp[threadNumber][left,right][calibCircle][vectorOfPoints]
				if (data->pupilPos[0]!=Point(0,0) && calib_tmp[data->id][0][calibprocess].size()<=10) {calib_tmp[data->id][0][calibprocess].push_back(data->pupilPos[0]);}
				if (data->pupilPos[1]!=Point(0,0) && calib_tmp[data->id][1][calibprocess].size()<=10) {calib_tmp[data->id][1][calibprocess].push_back(data->pupilPos[1]);}
				cout << calibprocess << " - - " << calib_tmp[data->id][1][calibprocess].size() << endl;
				int nextStep=1;
				for (int i=0;i<receivedThreads.size();i++)
				{if (calib_tmp[receivedThreads[i]][0][calibprocess].size()<10 || calib_tmp[receivedThreads[i]][1][calibprocess].size()<10) {nextStep=0;}}
				if (nextStep==1) {calibprocess++;counter=0;}
			}
			if (calibprocess==9)
			{
				// calculate standard values for each thread ...
				for (int i=0;i<receivedThreads.size();i++)
				{
					// ...and each eye...
					for (int j=0;j<1;j++)
					{
						// ...and each process
						for (int k=0;k<9;k++)
						{
							int x=0,y=0;
							// through all values in vector
							for (int l=0;l<calib_tmp[receivedThreads[i]][j][k].size();l++)
							{
								x+=calib_tmp[receivedThreads[i]][j][k][l].x;
								y+=calib_tmp[receivedThreads[i]][j][k][l].y;
							}
							x=(int)((float)x/(float)(calib_tmp[receivedThreads[i]][j][k].size()-1));
							y=(int)((float)y/(float)(calib_tmp[receivedThreads[i]][j][k].size()-1));
						}
					}
				}

			}
			// get data package from other thread
			//cout << data->x;
			// set event to tell other thread to go on
	
			ThreadCom_reply(msg);
			//			if ((char*)msg.lParam == "threadId") {cout << (unsigned long)msg.wParam;} //Display what was received in wParam of the message<br/>
		
			if (calibprocess<9 && (data->pupilPos[0]!=Point(0,0) || data->pupilPos[1]!=Point(0,0))) {counter++;}
		imshow("output",base);
		}
}
