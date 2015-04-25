#pragma once
#include "eyeTracker.h" // access of extern global Externals
#include <queue>
#include "opencv2/opencv.hpp"

class DetectionPupil
{
public:
	DetectionPupil(void);
	~DetectionPupil(void);

	Point findEyeCenter(Mat*,Rect,int);
	void advThreshold(Mat);
	Point detect(Mat);
	void scaleToFastSize(const Mat&,Mat&);
	Mat floodKillEdges(Mat&);
	bool floodShouldPushPoint(const Point&, const Mat&);
	bool inMat(Point,int,int);
	Point unscalePoint(Point, Point);
	void testPossibleCentersFormula(int, int, unsigned char,double, double,Mat&);
	Mat computeMatXGradient(const Mat &);
	Mat matrixMagnitude(const Mat&, const Mat&);
	double computeDynamicThreshold(const Mat&, double);

	Point calcHelper(Mat,int);
	bool nearlyEqual(int,int,int);

private:
	int first;
	Mat _working_frame;
	vector<Mat> firstMat;
	vector<Point> firstPup;

	Point _helper;
	Rect _eyeRect;
};

