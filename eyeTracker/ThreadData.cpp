#include "ThreadData.h"

ThreadData::ThreadData(void) 
{
	dim1=640,dim2=360;
	namedWindow("output", CV_WINDOW_NORMAL);
	resizeWindow("output",1,1);
	//setWindowProperty("output", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
	Thread();
	Start();
	base=Mat(dim2,dim1, CV_32F);
	rectangle(base,Point(0,0),Point(dim1,dim2),Scalar(255,255,255),CV_FILLED);
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

#define CALIBCIRCLES 25

void ThreadData::showGazeWindow(){

	gazeTracking = true;
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
		ThreadCom_reply(msg);
		//base=Mat(dim2,dim1, CV_32F);
		//white background
		rectangle(base,Point(0,0),Point(dim1,dim2),Scalar(255,255,255),CV_FILLED);
		data = (dataPackage*)msg.wParam;

		//if(data->gazeTracking == true && gazeTracking == true){
		//	showGazeWindow();
		//}
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
			if (data->pupilPos[0]!=Point(0,0) && calib_tmp[data->id][0][calibprocess].size()<=CALIBCIRCLES) {calib_tmp[data->id][0][calibprocess].push_back(data->pupilPos[0]);}
			if (data->pupilPos[1]!=Point(0,0) && calib_tmp[data->id][1][calibprocess].size()<=CALIBCIRCLES) {calib_tmp[data->id][1][calibprocess].push_back(data->pupilPos[1]);}
			cout << calibprocess << " - - " << calib_tmp[data->id][1][calibprocess].size() << endl;
			int nextStep=1;
			for (int i=0;i<receivedThreads.size();i++)
			{if (calib_tmp[receivedThreads[i]][0][calibprocess].size()<CALIBCIRCLES || calib_tmp[receivedThreads[i]][1][calibprocess].size()<CALIBCIRCLES) {nextStep=0;}}
			if (nextStep==1 && calibprocess<9) {calibprocess++;counter=0;}
			//if (nextStep==1 && calibprocess==0) {calibprocess+=8;counter=0;}
			//else if (nextStep==1 && calibprocess==8) {calibprocess=9;counter=0;}
		}
		if (calibprocess==9)
		{
			int x1=0;int count=0;int y1=0;
			// calculate standard values for each thread ...
			for (int i=0;i<receivedThreads.size();i++)
			{
				// ...and each eye...
				for (int j=0;j<2;j++)
				{
					// ...and each calibprocess
					for (int k=0;k<9;k++)
					{
						int x=0,y=0;
						// through all pupil in vector
						for (int l=0;l<calib_tmp[receivedThreads[i]][j][k].size();l++)
						{
							x+=calib_tmp[receivedThreads[i]][j][k][l].x;
							y+=calib_tmp[receivedThreads[i]][j][k][l].y;
						}
						int x1 = (int)((float)x/(float)(calib_tmp[receivedThreads[i]][j][k].size()));
						int y1 = (int)((float)y/(float)(calib_tmp[receivedThreads[i]][j][k].size()));
						x=0;y=0;
						int average = calib_tmp[receivedThreads[i]][j][k].size();
						for (int l=0;l<calib_tmp[receivedThreads[i]][j][k].size();l++)
						{
							if (calib_tmp[receivedThreads[i]][j][k][l].x >= x1*0.9 &&  calib_tmp[receivedThreads[i]][j][k][l].x <= x1*1.1 &&
								calib_tmp[receivedThreads[i]][j][k][l].y >= y1*0.9 &&  calib_tmp[receivedThreads[i]][j][k][l].y <= y1*1.1)
							{
								x+=calib_tmp[receivedThreads[i]][j][k][l].x;
								y+=calib_tmp[receivedThreads[i]][j][k][l].y;
							}
							else
							{
								average--;
							}
						}
						calib[receivedThreads[i]][j][k].x=(int)((float)x/(float)(average));
						calib[receivedThreads[i]][j][k].y=(int)((float)y/(float)(average));
					}
					x1=0;y1=0;
					x1+=calib[receivedThreads[i]][j][2].x-calib[receivedThreads[i]][j][1].x;
					x1+=calib[receivedThreads[i]][j][1].x-calib[receivedThreads[i]][j][0].x;
					x1+=calib[receivedThreads[i]][j][5].x-calib[receivedThreads[i]][j][4].x;
					x1+=calib[receivedThreads[i]][j][4].x-calib[receivedThreads[i]][j][3].x;
					x1+=calib[receivedThreads[i]][j][8].x-calib[receivedThreads[i]][j][7].x;
					x1+=calib[receivedThreads[i]][j][7].x-calib[receivedThreads[i]][j][6].x;
					calib2[receivedThreads[i]][j].x=100*x1/6;
					y1+=calib[receivedThreads[i]][j][6].y-calib[receivedThreads[i]][j][3].y;
					y1+=calib[receivedThreads[i]][j][3].y-calib[receivedThreads[i]][j][0].y;
					y1+=calib[receivedThreads[i]][j][7].y-calib[receivedThreads[i]][j][4].y;
					y1+=calib[receivedThreads[i]][j][4].y-calib[receivedThreads[i]][j][1].y;
					y1+=calib[receivedThreads[i]][j][8].y-calib[receivedThreads[i]][j][5].y;
					y1+=calib[receivedThreads[i]][j][5].y-calib[receivedThreads[i]][j][2].y;
					calib2[receivedThreads[i]][j].y=100*y1/6;						
				}
			}
			calibprocess++;
		}
		// finally check datas that come with calibrationvalues
		if (calibprocess==10)
		{
			int x=0,y=0;
			for (int m=0;m<2;m++)
			{
				if (data->pupilPos[m]!=Point(0,0))
				{
					int point=data->pupilPos[m].x-calib[data->id][m][0].x;
					int difBetweenFirstNLast=calib2[data->id][m].x*2;
					//int difBetweenFirstNLast=calib[data->id][m][8].x-calib[data->id][m][0].x;
					float percentage = (((float)point/((float)difBetweenFirstNLast/100.0)));
					x = ((int)(dim1*percentage));
					point=data->pupilPos[m].y-calib[data->id][m][0].y;
					difBetweenFirstNLast=calib2[data->id][m].y*2;
					//difBetweenFirstNLast=calib[data->id][m][8].y-calib[data->id][m][0].y;
					percentage = (((float)point/((float)difBetweenFirstNLast/100.0)));
					y = ((int)(dim2*percentage));
				}
			}
			if (x>=0 && y>=0)
			{circle(base,Point(x,y),dim2/100,Scalar(0,0,0),CV_FILLED);}
		}
		// get data package from other thread
		//cout << data->x;
		// set event to tell other thread to go on
		//			if ((char*)msg.lParam == "threadId") {cout << (unsigned long)msg.wParam;} //Display what was received in wParam of the message<br/>

		if (calibprocess<9 && (data->pupilPos[0]!=Point(0,0) || data->pupilPos[1]!=Point(0,0))) {counter++;}

		if(data->gazeTracking){
			setWindowProperty("output", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
		}
		imshow("output",base);

	}
}
