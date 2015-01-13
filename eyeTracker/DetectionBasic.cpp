// todo: eyes must be detected inside the eyes[x]
// previousface value for check if face is nearly the same.
// dont know exactly, if previous eyes are needed. Otherwise the existing eyes can be aken
#include "DetectionBasic.h"


DetectionBasic::DetectionBasic(void) : _distanceOfPupils(0),_distanceOfPupilsCounter(0)
{
	_face_cascade.read(files->getFaceCascade());
	_eye_cascade.read(files->getEyeCascade());
	eyes.push_back(Rect(0,0,0,0));
	eyes.push_back(Rect(0,0,0,0));
	pupil.push_back(Point(0,0));
	pupil.push_back(Point(0,0));
}


DetectionBasic::~DetectionBasic(void)
{
}


void DetectionBasic::detect(Mat* frame)
{
	_frame=frame;
	if (detectFace())
	{
		calculateEyeAreas();
		detectEye();
	}
	detectPupils();
}

void DetectionBasic::detectPupils()
{
	for (int i=0;i<eyes.size();i++)
	{
		// detecting the pupil in the eyeArea-part and adding x&y of beginning of eyeArea
		if (eyes[i] != Rect(0,0,0,0)) {pupil[i]=detPup.findEyeCenter(_frame,eyes[i]);}
	}
	if (pupil.size()==2)
	{
		if (_distanceOfPupils == 0)
		{_distanceOfPupils=sqrt(pow((pupil[1].x-pupil[0].x),2)+pow((pupil[1].x-pupil[0].x),2));}
		else 
		{
			if (nearlyEqual(_distanceOfPupils,sqrt(pow((pupil[1].x-pupil[0].x),2)+pow((pupil[1].x-pupil[0].x),2)),5))
			{
				_distanceOfPupilsCounter=0;
				_distanceOfPupils=sqrt(pow((pupil[1].x-pupil[0].x),2)+pow((pupil[1].x-pupil[0].x),2));
			}
			else
			{
				pupil[0]=Point(0,0);
				pupil[1]=Point(0,0);
				_distanceOfPupilsCounter++;
				if (_distanceOfPupilsCounter>=5) {_distanceOfPupils=0;}
			}
		}
	}
}	
			
void DetectionBasic::detectEye()
{
	// for each eye area
	for (int i=0;i<2;i++)
	{
	vector<Rect> _eyes; //for results of cascadefiles
	// detect eyes inside the calculated eye area field and fill array
	//Mat _frame_eyeArea;
	cvtColor( (*_frame)(eyes[i]), _working_frame, CV_BGR2GRAY );
	equalizeHist( _working_frame, _working_frame );
	GaussianBlur( _working_frame, _working_frame, cv::Size( 5, 5 ), 1);
	_eye_cascade.detectMultiScale(_working_frame, _eyes, 1.05, 3, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );
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
	eyes[0].x=(int)((double)face.width*0.10)+face.x;       
	eyes[1].x=(int)((double)face.width * 0.50) +face.x;
    for (int i=0;i<2;i++)
	{
	eyes[i].y = (int)((double)face.height * 0.25)+face.y;
    eyes[i].height = (int)((double)face.height * 0.3);
    eyes[i].width = (int)((double)face.width * 0.4);
	}
}

bool DetectionBasic::detectFace()
{
	//Mat __frame_edited;
	cvtColor( *_frame, _working_frame, CV_BGR2GRAY );
	// resize face image for faster results
	double widthFactor=_frame->cols/320.00;
	double heightFactor=_frame->rows/240.00;
	resize(_working_frame,_working_frame,Size(int(_working_frame.cols/widthFactor),int(_working_frame.rows/heightFactor)), 0, 0, INTER_LINEAR );
	vector<Rect> _faces; //for results of cascadefiles
	_face_cascade.detectMultiScale(_working_frame, _faces, 1.1, 3, 0|CV_HAAR_SCALE_IMAGE, Size(50, 50) );
	// if a face was detected
	if(_faces.size()>0)
	{
	// 1.resize back
	_faces[0].x=(int)((double)_faces[0].x*widthFactor);
	_faces[0].y=(int)((double)_faces[0].y*heightFactor);
	_faces[0].width=(int)((double)_faces[0].width*widthFactor);
	_faces[0].height=(int)((double)_faces[0].height*heightFactor);
	// 2. check if it the same face like before, then no new face detected, else return new face
	if (nearlyEqual(face.width,_faces[0].width,5) && nearlyEqual(face.height,_faces[0].height,5) && 
		nearlyEqual(face.x,_faces[0].x,5) && nearlyEqual(face.y,_faces[0].y,5))
		{return false;}
	else {face=_faces[0];return true;}
	}
	else {setRectangleZero(face);return false;}
}


 Rect* DetectionBasic::getFaceRect()
 {
	 return &face;
 }

 Rect* DetectionBasic::getEyeRect(int position)
 {
	 return &eyes[position];
 }

  Point* DetectionBasic::getPupilPoint (int position)
 {
	 return &pupil[position];
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

bool DetectionBasic::nearlyEqual(int x,int y,int z)
{
	if (x <= (1.00+(((double)z)/100))*y && x >= (1.00-(((double)z)/100))*y) {return true;}
	else {return false;}
}

