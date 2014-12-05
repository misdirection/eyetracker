#pragma once
#include "Thread.h"


class DetectionCircles
{
public:
	DetectionCircles(void);
	~DetectionCircles(void);
	// main function
	void detect(Mat*, Rect*);
	// area of face 
	bool calculateCircleArea(Rect*);
	// circleDetection
	bool detectCircles();
	// calculate the distance between detected circles 
	void calculateDistanceBetweenPoints();

	bool fillCircleMatrix();
	bool fillCircleMatrix_matrixPossible();
	bool fillCircleMatrix_result1();
	bool fillCircleMatrix_result2();
	bool fillCircleMatrix_matrixFilled();

	// int1-3: circle points, int4: which matrix in circleMatrix
	bool calc3rdOutOf2(int,int,int,int);
	void calcDistances();
	void calcDistances_help(int,int,int,int,int);
	//bool calculate_distanceBetweenPointsInMatrixAreNearlySame();
	bool setNeighbors(int,int);
	//void calcBasicRotation();
	//getter
	Rect* getCircleArea();
	Point getCoordsOfcircleMatrix(int);
	double getRotationAngle(int);
	double getDistanceBetweenPoints();
	// helper
	bool nearlyEqual(int,int,int);
	int getNeighborInMatrix(int,int);

private:
	Mat* _frame;
	Mat _working_frame;
	vector<double> _rotationOfCalib;
	vector<double> _rotationCurrentFrame;
	int _noDetectionCount;
	// distance between 2 points in pixels (length)
	double _distanceBetweenPoints;
	// distance between points after filling the matrix
	map<int,vector<Point>> _distanceOfPoints_tmp;
	map<int,double> _distOfPoints;

	vector<Point> circles;
	double distanceOfPoints(Point&,Point&);
	// coords of the circleMatrix
	map<int,map<int,Point*> > circleMatrix;
	map<int,Point> circleMatrixOld;
	Rect circleArea;


	// helpers
	vector<double> _distanceBetweenPointsTopResults_temp;
	vector<double> _distanceBetweenPointsInMatrix;
};

