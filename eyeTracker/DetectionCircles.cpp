#include "DetectionCircles.h"
#include <math.h>

DetectionCircles::DetectionCircles(void) :_distanceBetweenPoints(0),_rotationOfCalib(0),_noDetectionCount(0)
{
	_rotationCurrentFrame.push_back(0);
	_rotationCurrentFrame.push_back(0);
}


DetectionCircles::~DetectionCircles(void)
{
}

void DetectionCircles::detect(Mat* frame, Rect* face)
{
	_frame=frame;
	if (calculateCircleArea(face))
	{
	circles.clear();circleMatrix[0].clear();
	// if at least 4 circles were recognized
	if (detectCircles())
	{
		// no basic distance is calculated
		if (_distanceBetweenPoints == 0) {calculateDistanceBetweenPoints();}
		// if basic distance exists, but no first matrix creations for getting different x AND y distances
		else if (fillCircleMatrix())
		{
			if (_rotationOfCalib.empty() && _distOfPoints[0]!=0.0 && _distOfPoints[3]!=0) 
			{	
				// setup basic rotation
				_rotationOfCalib.push_back(atan2(_distOfPoints[1],_distOfPoints[0])*180/CV_PI);
				_rotationOfCalib.push_back(atan2(_distOfPoints[3],_distOfPoints[2])*180/CV_PI);
			}
			else 
			{
				_rotationCurrentFrame[0]= atan2(_distOfPoints[1],_distOfPoints[0])*180/CV_PI-_rotationOfCalib[0];
				_rotationCurrentFrame[1]= atan2(_distOfPoints[3],_distOfPoints[2])*180/CV_PI-_rotationOfCalib[1];
			}
		}
	}
	// now I have a basic value for first distance calculation.
	// we need also the rotation of the points in straight view to cam.
	// Now I have to create a matrix, that will be moved with the help of at least 3 detected points (2 cols,1 row or 1row,2cols)
	// with these 3 points we can calculate the other 6 points
	// if same rotation like straight view, distance can be can be calculated by change in distance between the points
	// first aim keep straight view and closer and farer (pixel count change) & drawing matrix
	// eye reg relative to headband
	// stürzt ab bei 3 cams oder mehr
	// min und mx punkte für ecken bestimmen.
	}
}

double DetectionCircles::getRotationAngle(int x)
{
	return _rotationCurrentFrame[x];
}

void DetectionCircles::calculateDistanceBetweenPoints()
{
	// all available distances are saved in this vector from all points to all points
	vector<double> _distancesBetweenAllCircles;
	//check all circles
	if (circles.size()>=5)
	{
		for(int i=0;i<circles.size();i++)
		{
			for(int j=i+1;j<circles.size();j++)
			{
				// distance between points
				_distancesBetweenAllCircles.push_back(distanceOfPoints(circles[i],circles[j]));
			}
		}
		std::sort(_distancesBetweenAllCircles.begin(),_distancesBetweenAllCircles.end());
		// drop smallest value (i=0) and use all that are smaller than the smallest possible distance diagonal
		double distanceSum = 0;
		int count=0;
		for(int i=1;i<_distancesBetweenAllCircles.size();i++)
		{
			if (_distancesBetweenAllCircles[i] < _distancesBetweenAllCircles[1]*sqrt(2)) {distanceSum+=_distancesBetweenAllCircles[i];count++;}
		}
		_distanceBetweenPointsTopResults_temp.push_back(distanceSum/count);
	}
	// if 3 results, then 
	if (_distanceBetweenPointsTopResults_temp.size() == 3)
	{
		if (nearlyEqual(_distanceBetweenPointsTopResults_temp[0],_distanceBetweenPointsTopResults_temp[1],20)
			&& nearlyEqual(_distanceBetweenPointsTopResults_temp[1],_distanceBetweenPointsTopResults_temp[2],20)
			&& nearlyEqual(_distanceBetweenPointsTopResults_temp[2],_distanceBetweenPointsTopResults_temp[0],20))
		{_distanceBetweenPoints=(_distanceBetweenPointsTopResults_temp[0]+_distanceBetweenPointsTopResults_temp[1]+_distanceBetweenPointsTopResults_temp[2])/3;}
		_distanceBetweenPointsTopResults_temp.clear();
	}
}

Point DetectionCircles::getCoordsOfcircleMatrix(int x)
{
	if (circleMatrix[0][x] != nullptr)
	{	return *circleMatrix[0][x]+Point(circleArea.x,circleArea.y);}
	else  {return Point(0,0);}
}

int DetectionCircles::getNeighborInMatrix(int i,int side)
{
	// 0=left
	if (side == 0 && i%3 != 0){i--;}
	// 1=right
	if (side == 1 && i%3 != 2){i++;}
	//2=above
	if (side == 2 && i> 2){i=i-3;}
	//3=under
	if (side == 3 && i< 6){i=i+3;}
	return i;
}

bool DetectionCircles::calc3rdOutOf2(int a,int b,int c,int inMatrix)
{
bool change=false;
if(circleMatrix[inMatrix][a]==nullptr && circleMatrix[inMatrix][b]!=nullptr && circleMatrix[inMatrix][c]!=nullptr)
{
	circleMatrix[inMatrix][a]= new Point(circleMatrix[inMatrix][b]->x-(circleMatrix[inMatrix][c]->x-circleMatrix[inMatrix][b]->x),
										 circleMatrix[inMatrix][b]->y-(circleMatrix[inMatrix][c]->y-circleMatrix[inMatrix][b]->y));
change=true;
}
else if(circleMatrix[inMatrix][a]!=nullptr && circleMatrix[inMatrix][b]==nullptr && circleMatrix[inMatrix][c]!=nullptr)
{
	circleMatrix[inMatrix][b]= new Point(circleMatrix[inMatrix][a]->x+(0.5*(circleMatrix[inMatrix][c]->x-circleMatrix[inMatrix][a]->x)),
										 circleMatrix[inMatrix][a]->y+(0.5*(circleMatrix[inMatrix][c]->y-circleMatrix[inMatrix][a]->y)));
change=true;
}
else if(circleMatrix[inMatrix][a]!=nullptr && circleMatrix[inMatrix][b]!=nullptr && circleMatrix[inMatrix][c]==nullptr)
{
	circleMatrix[inMatrix][c]= new Point(circleMatrix[inMatrix][b]->x+(circleMatrix[inMatrix][b]->x-circleMatrix[inMatrix][a]->x),
										 circleMatrix[inMatrix][b]->y+(circleMatrix[inMatrix][b]->y-circleMatrix[inMatrix][a]->y));
change=true;
}
return change;
}


	// calculate existing distances
void DetectionCircles::calcDistances()
{
	_distanceOfPoints_tmp.clear();
	calcDistances_help(0,1,2,0,0);calcDistances_help(3,4,5,0,0);calcDistances_help(6,7,8,0,0);calcDistances_help(0,3,6,1,0);calcDistances_help(1,4,7,1,0);calcDistances_help(2,5,8,1,0);
	_distOfPoints[0]=0;_distOfPoints[1]=0;_distOfPoints[2]=0;_distOfPoints[3]=0;
	for(int i=0;i<2;i++)
	{
		for (int j=0;j<_distanceOfPoints_tmp[i].size();j++)
		{
			_distOfPoints[2*i]+=_distanceOfPoints_tmp[i][j].x;
			_distOfPoints[2*i+1]+=_distanceOfPoints_tmp[i][j].y;
		}
		_distOfPoints[2*i]/=_distanceOfPoints_tmp[i].size();
		_distOfPoints[2*i+1]/=_distanceOfPoints_tmp[i].size();
	}
}

void DetectionCircles::calcDistances_help(int a,int b,int c,int direction, int inMatrix)
{
	if (circleMatrix[inMatrix][a]!=nullptr && circleMatrix[inMatrix][b]!=nullptr)
	{
		_distanceOfPoints_tmp[direction].push_back(*circleMatrix[inMatrix][b]-*circleMatrix[inMatrix][a]);
	}
	if (circleMatrix[inMatrix][b]!=nullptr && circleMatrix[inMatrix][c]!=nullptr)
	{
		_distanceOfPoints_tmp[direction].push_back(*circleMatrix[inMatrix][c]-*circleMatrix[inMatrix][b]);
	}
	if (circleMatrix[inMatrix][a]!=nullptr && circleMatrix[inMatrix][c]!=nullptr)
	{
		_distanceOfPoints_tmp[direction].push_back(0.5*(*circleMatrix[inMatrix][c]-*circleMatrix[inMatrix][a]));
	}
}

bool DetectionCircles::setNeighbors(int cir,int inMatrix)
{
	bool change=false;
	if (circleMatrix[inMatrix][cir]!=nullptr) 
	{
		// check if all neighbors have values
		if (circleMatrix[inMatrix][getNeighborInMatrix(cir,0)] == nullptr) 
		{
			circleMatrix[inMatrix][getNeighborInMatrix(cir,0)]=new Point(circleMatrix[inMatrix][cir]->x-_distOfPoints[0],
																		circleMatrix[inMatrix][cir]->y-_distOfPoints[1]);
			change=true;
		}
		if (circleMatrix[inMatrix][getNeighborInMatrix(cir,1)] == nullptr) 
		{
			circleMatrix[inMatrix][getNeighborInMatrix(cir,1)]=new Point(circleMatrix[inMatrix][cir]->x+_distOfPoints[0],
																		circleMatrix[inMatrix][cir]->y+_distOfPoints[1]);
			change=true;
		}
		if (circleMatrix[inMatrix][getNeighborInMatrix(cir,2)] == nullptr) 
		{
			circleMatrix[inMatrix][getNeighborInMatrix(cir,2)]=new Point(circleMatrix[inMatrix][cir]->x-_distOfPoints[2],
																		circleMatrix[inMatrix][cir]->y-_distOfPoints[3]);
			change=true;
		}
		if (circleMatrix[inMatrix][getNeighborInMatrix(cir,3)] == nullptr) 
		{
			circleMatrix[inMatrix][getNeighborInMatrix(cir,3)]=new Point(circleMatrix[inMatrix][cir]->x+_distOfPoints[2],
																		circleMatrix[inMatrix][cir]->y+_distOfPoints[3]);
			change=true;
		}
	}
	return change;
}


bool DetectionCircles::fillCircleMatrix_matrixPossible()
{
	// getting corner points
	// set first circle in every corner
	// get xMin and yMin
	int xMin=circles[0].x,yMin=circles[0].y,xMax=circles[0].x,yMax=circles[0].y;
	for (int i=1;i<circles.size();i++)
	{
		if (circles[i].x < xMin) {xMin=circles[i].x;}
		if (circles[i].y < yMin) {yMin=circles[i].y;}
		if (circles[i].x > xMax) {xMax=circles[i].x;}
		if (circles[i].y > yMax) {yMax=circles[i].y;}
	}
	int xMid=xMin+(xMax-xMin)/2;
	int yMid=yMin+(yMax-yMin)/2;
	// if only distanceBetweenPoints exists
	// distance must be nearly the same like in calibration process
	if ((_distOfPoints[0]==0 && _distOfPoints[1]==0) || (_distOfPoints[2]==0 && _distOfPoints[3]==0))	
	{
	if (!nearlyEqual(distanceOfPoints(Point(xMid,yMin),Point(xMin,yMin)),_distanceBetweenPoints,40) || !nearlyEqual(distanceOfPoints(Point(xMin,yMid),Point(xMin,yMin)),_distanceBetweenPoints,40) ) {return false;}
	}
	else
	{
		// distance must be nearly the one from previous
		if ((!nearlyEqual(distanceOfPoints(Point(xMid,yMin),Point(xMin,yMin)),distanceOfPoints(Point(0,0),Point(_distOfPoints[0],_distOfPoints[1])),40) || !nearlyEqual(distanceOfPoints(Point(xMin,yMid),Point(xMin,yMin)),distanceOfPoints(Point(0,0),Point(_distOfPoints[2],_distOfPoints[3])),40))) 
		{
			_noDetectionCount++;
			if (_noDetectionCount==10) {_noDetectionCount=0;_distOfPoints[0]=0;_distOfPoints[1]=0;_distOfPoints[2]=0;_distOfPoints[3]=0;}
			return false;
		}
	}
	bool row0=false,row1=false,row2=false,col0=false,col1=false,col2=false;int count=0;
	// fill matrix with detected circles
	for (int i=0;i<circles.size();i++)
	{
		if (nearlyEqual(circles[i].x,xMin,5) && nearlyEqual(circles[i].y,yMin,5)) {circleMatrix[0][0]=&circles[i];row0=true;col0=true;count++;}
		if (nearlyEqual(circles[i].x,xMid,5) && nearlyEqual(circles[i].y,yMin,5)) {circleMatrix[0][1]=&circles[i];row0=true;col1=true;count++;}
		if (nearlyEqual(circles[i].x,xMax,5) && nearlyEqual(circles[i].y,yMin,5)) {circleMatrix[0][2]=&circles[i];row0=true;col2=true;count++;}
		if (nearlyEqual(circles[i].x,xMin,5) && nearlyEqual(circles[i].y,yMid,5)) {circleMatrix[0][3]=&circles[i];row1=true;col0=true;count++;}
		if (nearlyEqual(circles[i].x,xMid,5) && nearlyEqual(circles[i].y,yMid,5)) {circleMatrix[0][4]=&circles[i];row1=true;col1=true;count++;}
		if (nearlyEqual(circles[i].x,xMax,5) && nearlyEqual(circles[i].y,yMid,5)) {circleMatrix[0][5]=&circles[i];row1=true;col2=true;count++;}
		if (nearlyEqual(circles[i].x,xMin,5) && nearlyEqual(circles[i].y,yMax,5)) {circleMatrix[0][6]=&circles[i];row2=true;col0=true;count++;}
		if (nearlyEqual(circles[i].x,xMid,5) && nearlyEqual(circles[i].y,yMax,5)) {circleMatrix[0][7]=&circles[i];row2=true;col1=true;count++;}
		if (nearlyEqual(circles[i].x,xMax,5) && nearlyEqual(circles[i].y,yMax,5)) {circleMatrix[0][8]=&circles[i];row2=true;col2=true;count++;}
	}
	// if 2, add third
	// 3/2 or 2/3 rows/cols must have at least one value, check if we can go on
	if (count > 3  && ((row0 && row2 && col0 && col1 && col2) || (row0 && row1 && row2 && col0 && col2))) {return true;}
	else {return false;}
}

bool DetectionCircles::fillCircleMatrix_result1()
{
	// fill other matrixes for calculating ([1] and [2] needed
	for (int i=1;i<2;i++)
	{
	circleMatrix[i]=circleMatrix[0];
	}

	// first horizontal, then vertical is first result		
	while (calc3rdOutOf2(0,1,2,1) | calc3rdOutOf2(3,4,5,1) | calc3rdOutOf2(6,7,8,1) | calc3rdOutOf2(0,3,6,1) | calc3rdOutOf2(1,4,7,1) | calc3rdOutOf2(2,5,8,1));		
	// first vertical, then horizontal is second result
	while (calc3rdOutOf2(0,3,6,2) | calc3rdOutOf2(1,4,7,2) | calc3rdOutOf2(2,5,8,2) | calc3rdOutOf2(0,1,2,2) | calc3rdOutOf2(3,4,5,2) | calc3rdOutOf2(6,7,8,2));
	for (int i=0;i<9;i++)
	{
		// if its not a detected circle
		if (circleMatrix[0][i]!=(circleMatrix[1][i]))
		{
			// fill circleMatrix[1] as possible result 
			if (circleMatrix[1][i]==nullptr && circleMatrix[2][i]!=nullptr)
				{circleMatrix[1][i]=circleMatrix[2][i];}
			else if (circleMatrix[1][i]!=nullptr && circleMatrix[2][i]!=nullptr)
			{
				circleMatrix[1][i]->x=(circleMatrix[1][i]->x+circleMatrix[2][i]->x)/2;
				circleMatrix[1][i]->y=(circleMatrix[1][i]->y+circleMatrix[2][i]->y)/2;
			}
		}
	}
	circleMatrix[0]=circleMatrix[1];
	return true;
}

bool DetectionCircles::fillCircleMatrix_result2()
{
	
	// fill other matrixes for calculating ([1] till [4])
	for (int i=1;i<5;i++)
	{
	circleMatrix[i]=circleMatrix[0];
	}
	// create order of going through 
	map<int,map<int,int>> order;
	int order1[18] = {0,1,2,3,4,5,6,7,8,0,1,2,3,4,5,6,7,8},order2[18] = {0,3,6,1,4,7,2,5,8,0,3,6,1,4,7,2,5,8};
	// first horizontal 0-9, starting with first non-nullptr
	// eg: circle 0 & 1 are nullptr:  2,3,4,5,6,7,8,0,1
	int circleStart=0;
	while (circleMatrix[1][circleStart]==nullptr) {circleStart++;}
	for (int cir=0;cir<circleMatrix[1].size();cir++,circleStart++)
	{
		setNeighbors(order1[circleStart],1);
	}
	// opposite direction 9-0
	circleStart=0;
	while (circleMatrix[2][circleMatrix[2].size()-1-circleStart]==nullptr) {circleStart++;}
	for (int cir=circleMatrix[2].size()-1;cir>=0;cir--,circleStart++)
	{
		setNeighbors(order1[17-circleStart],2);
	}
	circleStart=0;
	while (circleMatrix[3][order2[circleStart]]==nullptr) {circleStart++;}
	for (int cir=0;cir<circleMatrix[3].size();cir++,circleStart++)
	{
		setNeighbors(order2[circleStart],3);
	}
	// opposite direction 9-0
	circleStart=0;
	while (circleMatrix[4][circleMatrix[4].size()-1-order2[circleStart]]==nullptr) {circleStart++;}
	for (int cir=circleMatrix[4].size()-1;cir>=0;cir--,circleStart++)
	{
		setNeighbors(order1[17-circleStart],4);
	}
	for (int i=0;i<circleMatrix[0].size();i++)
	{
		circleMatrix[0][i]=new Point(0,0);
		circleMatrix[0][i]->x=(circleMatrix[1][i]->x+circleMatrix[2][i]->x+circleMatrix[3][i]->x+circleMatrix[4][i]->x)/4;
		circleMatrix[0][i]->y=(circleMatrix[1][i]->y+circleMatrix[2][i]->y+circleMatrix[3][i]->y+circleMatrix[4][i]->y)/4;
	}

	return true;
}

bool DetectionCircles::fillCircleMatrix_matrixFilled()
{
	for (int i=0;i<9;i++)
	{
		if (circleMatrix[0][i]==nullptr) {return false;}
	}
	for (int i=0;i<9;i++)
	{
		circleMatrixOld[i]=*circleMatrix[0][i];
	}

	return true;
}


bool DetectionCircles::fillCircleMatrix()
{
	if (circleGridSuccessful)
	{
			circleMatrix[0][0]=&circles[6];
			circleMatrix[0][1]=&circles[3];
			circleMatrix[0][2]=&circles[0];
			circleMatrix[0][3]=&circles[7];
			circleMatrix[0][4]=&circles[4];
			circleMatrix[0][5]=&circles[1];
			circleMatrix[0][6]=&circles[8];
			circleMatrix[0][7]=&circles[5];
			circleMatrix[0][8]=&circles[2];
			calcDistances();
			return true;
	}
	// if 9 circles are detected, the matrix is already complete. Update the distances and return
	// filling recognized circles into the matrix and check if enough for calculation
	if (fillCircleMatrix_matrixPossible())
	{
		if (circleMatrix[0][4] != nullptr && nearlyEqual(circleMatrix[0][4]->x,circleMatrixOld[4].x,3) && nearlyEqual(circleMatrix[0][4]->y,circleMatrixOld[4].y,3)) 
		{
			for (int i=0;i<9;i++)
			{
					circleMatrix[0][i]=&circleMatrixOld[i];
			}
		}
		if (fillCircleMatrix_matrixFilled()) 
		{
			calcDistances();
			if (nearlyEqual(_distOfPoints[0],_distOfPoints[3],50)) {return true;}
			else {return false;}
		}

		// 1st result by adding third with the help of circleMatrix[1] and [2], result if successful in [0]
		if (fillCircleMatrix_result1() && nearlyEqual(_distOfPoints[0],_distOfPoints[3],40)) 
		{
			// calculate distances needed for result2 and if return
			calcDistances();
			if (fillCircleMatrix_matrixFilled()) {return true;}
			// if not successfull, try with 2nd method to fill [using circleMatrix[2]-[5]
			if (fillCircleMatrix_result2())
			{
				if (fillCircleMatrix_matrixFilled()) {return true;}
			}
		}
	}
	circleMatrix.clear();return false;
}



bool DetectionCircles::calculateCircleArea(Rect* face)
{
	//check if a face was already recognized
	if (face->x)
	{
	circleArea.x=(int)(face->x+0.25*(double)face->width);
	circleArea.y=max((int)(face->y-0.2*(double)face->height),0);
	circleArea.width=(int)(0.5*(double)face->width);
	circleArea.height=(int)(0.5*(double)face->height);
	return true;
}	
	else {return false;}
}


bool DetectionCircles::detectCircles()
{
	cvtColor( (*_frame)(circleArea), _working_frame, CV_BGR2GRAY );
	equalizeHist(_working_frame,_working_frame);
	Mat test;
	// circleGrid needs black circles on white background -> invert
	bitwise_not(_working_frame,test);
	circleGridSuccessful=findCirclesGrid(test,Size(3,3), circles, CALIB_CB_SYMMETRIC_GRID + CALIB_CB_CLUSTERING);
	// use openCV detection of circle grid
	if (circleGridSuccessful==true) 
	{
		//return true;
	}
	//threshold(_working_frame, _working_frame,  120, 255, CV_THRESH_BINARY );
	Canny(_working_frame, _working_frame, 200, 255, 3);
	//imshow("web cam device 0",_working_frame);
	vector<vector<Point>> contours;
	vector<Point> approx;
    findContours( _working_frame, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE );
for (int i = 0; i < contours.size(); i++)
{
    // Approximate contour with accuracy proportional
    // to the contour perimeter
    cv::approxPolyDP(Mat(contours[i]),approx,arcLength(Mat(contours[i]), true) * 0.02,true);

	if (approx.size() > 5)
	{
        // Detect and label circles
        double area = contourArea(contours[i]);
        cv::Rect r = boundingRect(contours[i]);
        int radius = r.width / 2;
        if (abs(1 - (((double)r.width / r.height)) <= 0.3) & (abs(1 - (area / (CV_PI * std::pow(radius, 2)))) <= 0.3))
        {
			// check if circle already in vector
			int x = r.x+r.width/2;
			int y = r.y+r.height/2;
			// add first circle. Also say that circle 0 is in row 2
			if (circles.empty()) 
			{
				circles.push_back(Point(x,y));
			}
			// add all unique circles
			bool exists=false;
			for(int i=0;i<circles.size();i++)
			{
				if (nearlyEqual(circles[i].x,x,5) && nearlyEqual(circles[i].y,y,5)) {exists=true;}
			}
			if (!exists) 
			{
				//add to circles the new one
				circles.push_back(Point(x,y));
			}
        }
    }
}

if (circles.size()<4)
{
	return false;
}
else {return true;}
}

 Rect* DetectionCircles::getCircleArea()
 {
	 return &circleArea;
 }


bool DetectionCircles::nearlyEqual(int x,int y,int z)
{
	if (x <= (1.00+(((double)z)/100))*y && x >= (1.00-(((double)z)/100))*y) {return true;}
	else {return false;}
}

double DetectionCircles::distanceOfPoints(Point& i,Point& j)
{
	return sqrt(pow((i.x-j.x),2)+pow((i.y-j.y),2));
}

double DetectionCircles::getDistanceBetweenPoints()
{
	// if point values are empty we use the _distance of 1st calibration process
	// else we get the horizontal distance
	if (_distOfPoints[0]==0 && _distOfPoints[1]==0)
	{
		return _distanceBetweenPoints;
	}
	else {return sqrt(pow(_distOfPoints[0],2)+pow(_distOfPoints[1],2));}
}