#pragma once
#include "eyeTracker.h" // access of extern global Externals
#include <queue>

class DetectionPupil
{
public:
	DetectionPupil(void);
	~DetectionPupil(void);

	Point findEyeCenter(Mat*,Rect,int);
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

	Point calcHelper(int);
	bool nearlyEqual(int,int,int);

private:
	Mat _working_frame;

	Point _helper;
	Rect _eyeRect;
};

