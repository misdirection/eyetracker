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
	namedWindow(_windowName, CV_WINDOW_AUTOSIZE);
}






// main method of the ThreadCapturing which runs after the StartCapture
void ThreadCapturing::Run()
{
	data->y=111;
	data->x=GetCurrentThreadId();
	// test of  transfer to data thread only string here, but can be used with anything else
	// send message of the thread to data thread
	ThreadCom_send(_dataThreadId,(WPARAM)data);
	fps framesPerSeconds;
	DetectionBasic det;
	DetectionCircles detCir;
	DetectionPupil detPupil;

	while(_running)
	{
		Mat frame;
		_captureDevice >> frame; // get a new frame from camera
		flip(frame,frame,1);
		det.detect(&frame);
		detCir.detect(&frame,det.getFaceRect());

		//stringstream text; text << framesPerSeconds.getFPS();
		stringstream text; text << detCir.getRotationAngle(0) << " | " << detCir.getRotationAngle(1);
		putText(frame,text.str(),cvPoint(30,30), FONT_HERSHEY_SIMPLEX,1,Scalar(255,255,0),1,8,false);
		rectangle( frame,*det.getFaceRect(), Scalar( 0, 255, 0 ), 1, 8, 0 );
		rectangle( frame,det.getEyeRect(0), Scalar( 0, 255, 0 ), 1, 8, 0 );
		rectangle( frame,det.getEyeRect(1), Scalar( 0, 255, 0 ), 1, 8, 0 );
		rectangle( frame,*detCir.getCircleArea(), Scalar( 0, 255, 0 ), 1, 8, 0 );
		circle(frame,detPupil.detectPupil(det.getEyeRect(0),&frame), 2, Scalar( 0, 255, 0 ), 1, 8, 0 );
		circle(frame,detPupil.detectPupil(det.getEyeRect(1),&frame), 2, Scalar( 0, 255, 0 ), 1, 8, 0 );


		//for(int x=0;x<(detCir.getCoordsOfcircleMatrix()).size();x++)
		{
			//circle(frame,*(detCir.getCoordsOfcircleMatrix())[x],5, Scalar( 255, 255, 0 ), 2, 8, 0 );
		}		

		line(frame,detCir.getCoordsOfcircleMatrix(3),detCir.getCoordsOfcircleMatrix(5), Scalar( 0, 255, 0 ), 1, 8, 0 );
		line(frame,detCir.getCoordsOfcircleMatrix(1),detCir.getCoordsOfcircleMatrix(7), Scalar( 0, 255, 0 ), 1, 8, 0 );
		imshow(_windowName, frame); //displays an image in the specified window
		//cout << "fps:" << framesPerSeconds.getFPS() << endl;
		//if(cvWaitKey(1) >= 0);
	}

}
