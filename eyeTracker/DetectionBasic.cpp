// todo: eyes must be detected inside the eyes[x]
// previousface value for check if face is nearly the same.
// dont know exactly, if previous eyes are needed. Otherwise the existing eyes can be aken
#include "DetectionBasic.h"


DetectionBasic::DetectionBasic(void)
{
	eyes.push_back(Rect(0,0,0,0));
	eyes.push_back(Rect(0,0,0,0));
}


DetectionBasic::~DetectionBasic(void)
{
}


void DetectionBasic::detect(Mat frame)
{
	_frame=frame;
	if (detectFace())
	{
		calculateEyeAreas();
		detectEye();
	}
}

void DetectionBasic::detectEye()
{
	// for each eye area
	for (int i=0;i<2;i++)
	{
	vector<Rect> _eyes; //for results of cascadefiles
	// detect eyes inside the calculated eye area field and fill array
	Mat _frame_eyeArea;
	equalizeHist( _frame(eyes[i]), _frame_eyeArea );
	//GaussianBlur( _frame_eyeArea, _frame_eyeArea, cv::Size( 5, 5 ), 1);
	files->getEyeCascade().detectMultiScale(_frame_eyeArea, _eyes, 1.2, 3, 0|CV_HAAR_SCALE_IMAGE, Size(10, 10) );
	// if an eye is detected inside this area, take this as new eye. otherwise we keep the calculated eye area
	if (_eyes.size()>0)
	{
		eyes[i].x+=_eyes[0].x;
	eyes[i].y+=_eyes[0].y;
	eyes[i].height=_eyes[0].height;
	eyes[i].width=_eyes[0].width;
	}
	}
}

void DetectionBasic::calculateEyeAreas()
{
	eyes[0].x=(int)((double)face.width*0.13)+face.x;       
	eyes[1].x=face.width - (int)((double)face.width * 0.35) - (int)((double)face.width * 0.13) +face.x;
    for (int i=0;i<2;i++)
	{
	eyes[i].y = (int)((double)face.height * 0.25)+face.y;
    eyes[i].height = (int)((double)face.height * 0.3);
    eyes[i].width = (int)((double)face.width * 0.3);
	}
}

bool DetectionBasic::detectFace()
{
	// resize face image for faster results
	double widthFactor=_frame.cols/320.00;
	double heightFactor=_frame.rows/240.00;
	Mat __frame_edited;
	cvtColor( _frame, _frame, CV_BGR2GRAY );
	resize(_frame,__frame_edited,Size(int(_frame.cols/widthFactor),int(_frame.rows/heightFactor)), 0, 0, INTER_LINEAR );
	vector<Rect> _faces; //for results of cascadefiles
	files->getFaceCascade().detectMultiScale(__frame_edited, _faces, 1.1, 3, 0|CV_HAAR_SCALE_IMAGE, Size(50, 50) );
	// if a face was detected
	if(_faces.size()>0)
	{
	// 1.resize back
	_faces[0].x=(int)((double)_faces[0].x*widthFactor);
	_faces[0].y=(int)((double)_faces[0].y*heightFactor);
	_faces[0].width=(int)((double)_faces[0].width*widthFactor);
	_faces[0].height=(int)((double)_faces[0].height*heightFactor);
	// 2. check if it the same face like before, then no new face detected, else return new face
	if (face.width <= 1.05*_faces[0].width && face.width >= 0.95*_faces[0].width
		&& face.height <= 1.05*_faces[0].height && face.height >= 0.95*_faces[0].height
		&& face.x <= 1.05*_faces[0].x && face.x >= 0.95*_faces[0].x
		&& face.y <= 1.05*_faces[0].y && face.y >= 0.95*_faces[0].y)
		{return false;}
	else {face=_faces[0];return true;}
	}
	else {setRectangleZero(face);return false;}
}


 Rect DetectionBasic::getFaceRect()
 {
	 return face;
 }

 Rect DetectionBasic::getEyeRect(int position)
 {
	 return eyes[position];
 }

 void DetectionBasic::setRectangleZero(Rect r)
 {
	r.x=0; 
	r.y=0;
	r.height=0;
	r.width=0;
 }
 
 bool DetectionBasic::isRectangleZero(Rect r)
 {
	 if (r.x==0 && r.y==0 && r.height==0 && r.width==0) {return true;}
	 else return false;
 }



