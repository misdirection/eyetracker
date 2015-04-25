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
	eye_tpl.push_back(Mat());
	eye_tpl.push_back(Mat());
	eye_bb.push_back(Rect(0,0,0,0));
	eye_bb.push_back(Rect(0,0,0,0));
}


DetectionBasic::~DetectionBasic(void)
{
}


void DetectionBasic::detect(Mat* frame)
{
    _frame=frame;
	//Mat testMat;
	//threshold(*_frame,testMat,30,255,THRESH_TOZERO);
	//imshow("test2",testMat);
	cv::Mat gray;
        cv::cvtColor(*frame, gray, CV_BGR2GRAY);
		//if ((eye_bb[0].width == 0 && eye_bb[0].height == 0) || (eye_bb[1].width == 0 && eye_bb[1].height == 0) )
		if ((eye_bb[0].width == 0 && eye_bb[0].height == 0) || (eye_bb[1].width == 0 && eye_bb[1].height == 0) )
		{
			detectEye1(gray, eye_tpl, eye_bb);
		}
        else 
        {
			trackEye1(gray, eye_tpl, eye_bb);
			detectPupils();
        }
	
/*
	_frame=frame;
	cvtColor( *_frame, _working_frame, CV_BGR2GRAY );
	if ((eyes[0].width == 0 && eyes[0].height == 0)  || (eyes[1].width == 0 && eyes[1].height == 0) )
	{
		detectEye1();
	}
	else 
	{
		trackEye1(_working_frame,eye_tpl[0],eyes[0]);
		//rectangle(_working_frame, eyes[0], Scalar( 255, 255, 0 ), 1, 8, 0 );
		//imshow("test2",_working_frame);
	}
*/
/*
	_frame=frame;
	if (detectFace())
	{
		calculateEyeAreas();
		//detectEye();
	}
	detectPupils();
	*/
}

int DetectionBasic::detectEye1(cv::Mat& im, vector<Mat>& tpl, vector<Rect>& rect)
{
    std::vector<cv::Rect> _faces, _eyes;
	_face_cascade.detectMultiScale(im, _faces, 1.1, 3, 0|CV_HAAR_SCALE_IMAGE, Size(50, 50) );

    if (_faces.size()>0)
    {
		face=_faces[0];
		calculateEyeAreas();
		for (int i=0;i<=1;i++)
		{
        _eye_cascade.detectMultiScale(im(eyes[i]), _eyes, 1.1, 2, 
                                     CV_HAAR_SCALE_IMAGE, cv::Size(20,20));
        if (_eyes.size())
        {
            rect[i] = _eyes[0] + cv::Point(eyes[i].x, eyes[i].y);
			rect[i] = Rect(rect[i].x+rect[i].width*0.25,rect[i].y+rect[i].height*0.25,rect[i].width/2,rect[i].height/2);
            tpl[i]  = im(rect[i]);
        }
		}
    }
    return eyes.size();
}
/*
int DetectionBasic::detectEye1()
{
    std::vector<cv::Rect> _faces,_eyes;
	_face_cascade.detectMultiScale(_working_frame, _faces, 1.1, 3, 0|CV_HAAR_SCALE_IMAGE, Size(50, 50) );
    if (_faces.size()>0)
	{
		face=_faces[0];
	}
	calculateEyeAreas();
	for (int i=0;i<=1;i++)
	{
        _eye_cascade.detectMultiScale(_working_frame(eyes[i]), _eyes, 1.1, 3, 0|CV_HAAR_SCALE_IMAGE, Size(20, 20) );
		if (_eyes.size())
        {
            eyes[i] = _eyes[0] + Point(eyes[i].x, eyes[i].y);
            eye_tpl[i]  = _working_frame(eyes[i]);
        }
	}
    return eyes.size();
}
*/

void DetectionBasic::trackEye1(cv::Mat& im, vector<Mat>& tpl, vector<Rect>& rect)
{
	for (int i=0;i<=1;i++)
	{
	cv::Size size(rect[i].width * 2, rect[i].height * 2);
    cv::Rect window(rect[i] + size - cv::Point(size.width/2, size.height/2));
    window &= cv::Rect(0, 0, im.cols, im.rows);

    cv::Mat dst(window.width - tpl[i].rows + 1, window.height - tpl[i].cols + 1, CV_32FC1);
    cv::matchTemplate(im(window), tpl[i], dst, CV_TM_SQDIFF_NORMED);
	
	//normalize( dst, dst, 0, 1, NORM_MINMAX, -1, Mat() );
    double minval, maxval;
    cv::Point minloc, maxloc;
    cv::minMaxLoc(dst, &minval, &maxval, &minloc, &maxloc);

    if (minval <= 0.2)
    {
        rect[i].x = window.x + minloc.x;
        rect[i].y = window.y + minloc.y;
    }
    else
        detectEye1(im,tpl,rect);
	}
}

/*
void DetectionBasic::trackEye1()
{
    for (int i=0;i<=1;i++)
	{
		cv::Size size(eyes[i].width * 2, eyes[i].height * 2);
		cv::Rect window(eyes[i] + size - cv::Point(size.width/2, size.height/2));

	    window &= cv::Rect(0, 0, _working_frame.cols, _working_frame.rows);

		cv::Mat dst(window.width - eye_tpl[i].rows + 1, window.height - eye_tpl[i].cols + 1, CV_32FC1);
		cv::matchTemplate(_working_frame(window), eye_tpl[i], dst, CV_TM_SQDIFF_NORMED);
			
		double minval, maxval;
		cv::Point minloc, maxloc;
		cv::minMaxLoc(dst, &minval, &maxval, &minloc, &maxloc);

	    if (minval <= 0.2)
		{
			eyes[i].x = window.x + minloc.x;
			eyes[i].y = window.y + minloc.y;
		}
		else
		{
			eyes[i].x = eyes[i].y = eyes[i].width = eyes[i].height = 0;
		}
	}
}
*/
void DetectionBasic::detectPupils()
{
	
	for (int i=0;i<eyes.size();i++)
	{
		// detecting the pupil in the eyeArea-part and adding x&y of beginning of eyeArea
		if (eyes[i] != Rect(0,0,0,0)) {pupil[i]=detPup.findEyeCenter(_frame,eye_bb[i],i);}
	}
	if (pupil[0]!=Point(0,0) && pupil[1]!=Point(0,0) && pupil.size()==2)
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
	GaussianBlur( _working_frame, _working_frame, cv::Size( 5, 5 ), 1);
	_eye_cascade.detectMultiScale(_working_frame, _eyes, 1.1, 3, 0|CV_HAAR_SCALE_IMAGE, Size(20, 20) );

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
	eyes[0].x=(int)((double)face.width*0.2)+face.x;       
	eyes[1].x=(int)((double)face.width * 0.55) +face.x;
    for (int i=0;i<2;i++)
	{
	eyes[i].y = (int)((double)face.height * 0.35)+face.y;
    eyes[i].height = (int)((double)face.height * 0.15);
    eyes[i].width = (int)((double)face.width * 0.25);
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

