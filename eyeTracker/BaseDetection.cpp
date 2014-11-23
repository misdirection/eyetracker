#include "BaseDetection.h"
BaseDetection::BaseDetection()
{
	face_cascade.load("haarcascade_frontalface_alt2.xml");
	eye_cascade.load("haarcascade_eye.xml");
}


BaseDetection::~BaseDetection(void)
{
}


//detects faces

vector<cv::Rect> BaseDetection::detect(Mat frame)
{
	// make face image smaller
	double widthFactor=frame.cols/320.00;
	double heightFactor=frame.rows/240.00;
	Mat frame_gray;
	Mat frame_resized;
	std::vector<cv::Mat> rgbChannels(3);
	split(frame, rgbChannels);
	frame_gray = rgbChannels[2];
	cvtColor( frame, frame_gray, CV_BGR2GRAY );
	//frame_resized = frame_gray;
	resize(frame_gray,frame_resized,Size(int(frame.cols/widthFactor),int(frame.rows/heightFactor)), 0, 0, INTER_LINEAR );
	equalizeHist( frame_resized, frame_resized );
	GaussianBlur( frame_resized, frame_resized, cv::Size( 0, 0 ), 1);
	//face_cascade.detectMultiScale( frame_resized, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );
	face_cascade.detectMultiScale(frame_resized, faces, 1.1, 3, 0|CV_HAAR_SCALE_IMAGE, Size(50, 50) );
	return faces;
}


Mat BaseDetection::drawRect(Mat frame){

	double widthFactor=frame.cols/320.00;
	double heightFactor=frame.rows/240.00;
	for( size_t i = 0; i < faces.size(); i++ )
	{
		faces[i].width*=(int)widthFactor;
		faces[i].x*=(int)widthFactor;
		faces[i].height*=(int)heightFactor;
		faces[i].y*=(int)heightFactor;
	}

	for( size_t i = 0; i < faces.size(); i++ )
	{
		rectangle( frame,faces[i], Scalar( 255, 0, 255 ), 4, 8, 0 );
	}

	return frame;
}




Mat BaseDetection::drawEllipse(Mat frame)
{
	for( size_t i = 0; i < faces.size(); i++ )
	{
		Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
		ellipse( frame, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
	}
	return frame;
}