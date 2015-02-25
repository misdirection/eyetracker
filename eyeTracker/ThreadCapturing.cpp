#include "ThreadCapturing.h"
#include "fps.h"
#include "DetectionBasic.h"
#include "DetectionCircles.h"
#include "DetectionPupil.h"


ThreadCapturing::ThreadCapturing(void){};
ThreadCapturing::~ThreadCapturing(void){
	destroyWindow(_windowName);
	delete data;
	data = nullptr;};
ThreadCapturing::ThreadCapturing(LPSTR windowName, VideoCapture captureDevice,int deviceNumber, DWORD dataThreadID) :
	Thread(), 
	_captureDevice(captureDevice),
	_dataThreadId(dataThreadID),
	_deviceNumber(deviceNumber)
{
	data=new dataPackage;
	strcpy_s(_windowName, windowName);
	_captureDevice.open(_deviceNumber);
	_captureDevice.set(CV_CAP_PROP_FRAME_WIDTH,1920);
	_captureDevice.set(CV_CAP_PROP_FRAME_HEIGHT,1280);
	namedWindow(_windowName, CV_WINDOW_NORMAL);
	resizeWindow(_windowName,640,480);
	namedWindow("test", CV_WINDOW_AUTOSIZE);
	namedWindow("test2", CV_WINDOW_AUTOSIZE);
}






// main method of the ThreadCapturing which runs after the StartCapture
void ThreadCapturing::Run()
{
    // test of  transfer to data thread only string here, but can be used with anything else
    // send message of the thread to data thread
    data->firstpackage = true;
    data->id=GetCurrentThreadId();
    data->pupilPos.push_back(Point(0,0));
    data->pupilPos.push_back(Point(0,0));
    ThreadCom_send(_dataThreadId,(WPARAM)data);

    fps framesPerSeconds;
    DetectionBasic det;
    DetectionCircles detCir;
	cout << _captureDevice.get(CV_CAP_PROP_FRAME_WIDTH)<< endl;	
    while(_running)
    {
        Mat frame;
        _captureDevice >> frame; // get a new frame from camera
		flip(frame,frame,1);
        det.detect(&frame);
        detCir.detect(&frame,det.getFaceRect());
        // confirm pupils by checking distance between circleGridCenter and pupils, must be nearly the same as before
        stringstream text; text << framesPerSeconds.getFPS();
        //stringstream text; text << detCir.getRotationAngle(0) << " | " << detCir.getRotationAngle(1);
        putText(frame,text.str(),cvPoint(30,30), FONT_HERSHEY_SIMPLEX,1,Scalar(255,255,0),1,8,false);
        rectangle( frame,*det.getFaceRect(), Scalar( 0, 255, 0 ), 1, 8, 0 );
        rectangle( frame,*det.getEyeRect(0), Scalar( 0, 255, 0 ), 1, 8, 0 );
        rectangle( frame,*det.getEyeRect(1), Scalar( 0, 255, 0 ), 1, 8, 0 );
        rectangle( frame,*detCir.getCircleArea(), Scalar( 0, 255, 0 ), 1, 8, 0 );
        circle(frame,*det.getPupilPoint(0), 2, Scalar( 0, 255, 0 ), 1, 8, 0 );
        circle(frame,*det.getPupilPoint(1), 2, Scalar( 0, 255, 0 ), 1, 8, 0 );

        for(int x=0;x<9;x++)
        {
            circle(frame,detCir.getCoordsOfcircleMatrix(x),5, Scalar( 255, 255, 0 ), 2, 8, 0 );
        }

        line(frame,detCir.getCoordsOfcircleMatrix(3),detCir.getCoordsOfcircleMatrix(5), Scalar( 0, 255, 0 ), 1, 8, 0 );
        line(frame,detCir.getCoordsOfcircleMatrix(1),detCir.getCoordsOfcircleMatrix(7), Scalar( 0, 255, 0 ), 1, 8, 0 );
        data->firstpackage = false;
        data->id=GetCurrentThreadId();
		if(detCir.getCoordsOfcircleMatrix(5)!=Point(0,0))
		{data->pupilPos[0] = *det.getPupilPoint(0)-detCir.getCoordsOfcircleMatrix(4)+Point(200,200);
		data->pupilPos[1] = *det.getPupilPoint(1)-detCir.getCoordsOfcircleMatrix(4)+Point(200,200);}
		else 
		{
			data->pupilPos[0]=Point(0,0);data->pupilPos[1]=Point(0,0);
		}
		if (data->pupilPos[0].x<=0 || data->pupilPos[0].y<=0 || data->pupilPos[1].x<=0 || data->pupilPos[1].y<=0)
		{data->pupilPos[0]=Point(0,0);data->pupilPos[1]=Point(0,0);}

        ThreadCom_send(_dataThreadId,(WPARAM)data);
        imshow(_windowName, frame); //displays an image in the specified window
        //cout << "fps:" << framesPerSeconds.getFPS() << endl;
        //if(cvWaitKey(1) >= 0);
    }

}

