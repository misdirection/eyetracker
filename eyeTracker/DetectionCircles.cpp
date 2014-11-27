#include "DetectionCircles.h"
#include <math.h>

DetectionCircles::DetectionCircles(void)
{
	_distanceBetweenPoints.push_back(0.0);
	_distanceBetweenPoints.push_back(0.0);
	basicAngle.push_back(0.0);
	basicAngle.push_back(0.0);
}


DetectionCircles::~DetectionCircles(void)
{
}


void DetectionCircles::calculateDistanceBetweenPoints()
{
	// also remove duplicates in this loop!
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
		_distanceBetweenPoints[0]=(_distanceBetweenPointsTopResults_temp[0]+_distanceBetweenPointsTopResults_temp[1]+_distanceBetweenPointsTopResults_temp[2])/3;
		_distanceBetweenPointsTopResults_temp.clear();
	}
}

void DetectionCircles::calcRotation()
{
	if (circleMatrix.size()==9)
	{
	// TODO: middle value of all, not only corners
	basicAngle[0] = atan2(circleMatrix[5].y-circleMatrix[3].y,circleMatrix[5].x-circleMatrix[3].x)*180/CV_PI;
	basicAngle[1] = atan2(circleMatrix[5].x-circleMatrix[3].x,circleMatrix[5].y-circleMatrix[3].y)*180/CV_PI;
	}
}

void DetectionCircles::detect(Mat* frame, Rect* face)
{
	_frame=frame;
	if (calculateCircleArea(face))
	{
	circles.clear();circleMatrix.clear();
	// if at least 4 circles were recognized
	if (detectCircles())
	{
		// no basic distance is calculated
		if (_distanceBetweenPoints[0] == 0.0) {calculateDistanceBetweenPoints();}
		// if basic distance exists, but no first matrix creations for getting different x&y distances
		if (_distanceBetweenPoints[1] == 0.0) 
		{	
			fillCircleMatrix();
			// fill basic rotation values
			// ebene (1,0)
			// vektor(circleMatrix[2].x-circleMatrix[0].x,circleMatrix[2].y-circleMatrix[0].y)
			//double dot = x1*x2 + y1*y2;
			if (circleMatrix.size() > 0 ) {calcRotation();}

			//double basicangle = atan2(circleMatrix[2].y-circleMatrix[0].y,circleMatrix[2].x-circleMatrix[0].x);
			//cout << basicangle*180/3.14 << endl;
		}
		//now use distanceBetweenPoints and focal length to calculate basic distance
		else {fillCircleMatrix();calcRotation();}
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

Point DetectionCircles::getCoordsOfcircleMatrix(int row, int col)
{
	return circleMatrix[3*row+col];
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

bool DetectionCircles::calculate_distanceBetweenPointsInMatrixAreNearlySame()
{
	_distanceBetweenPointsInMatrix.clear();
	_distanceBetweenPointsInMatrix.push_back(0.0);
	_distanceBetweenPointsInMatrix.push_back(0.0);
	_distanceBetweenPointsInMatrix.push_back(0.0);
	_distanceBetweenPointsInMatrix.push_back(0.0);
	_distanceBetweenPointsInMatrix.push_back(0.0);
	_distanceBetweenPointsInMatrix.push_back(0.0);
	vector<vector<double>>horiAndVert(6);
	for (int i=0;i<9;i++)
	{
		double _temp_dist,_temp_value,_temp_value2;
		_temp_dist=distanceOfPoints(circleMatrix[i],circleMatrix[getNeighborInMatrix(i,0)]);
		_temp_value=abs(circleMatrix[i].x-circleMatrix[getNeighborInMatrix(i,0)].x);
		_temp_value2=abs(circleMatrix[i].y-circleMatrix[getNeighborInMatrix(i,0)].y);
		if (_temp_dist != 0 && circleMatrix[i] != Point(0,0) && circleMatrix[getNeighborInMatrix(i,0)] != Point(0,0)) {horiAndVert[0].push_back(_temp_dist);horiAndVert[2].push_back(_temp_value);horiAndVert[3].push_back(_temp_value2);}
		_temp_dist=distanceOfPoints(circleMatrix[i],circleMatrix[getNeighborInMatrix(i,1)]);
		_temp_value=abs(circleMatrix[i].x-circleMatrix[getNeighborInMatrix(i,1)].x);
		_temp_value2=abs(circleMatrix[i].y-circleMatrix[getNeighborInMatrix(i,1)].y);
		if (_temp_dist != 0 && circleMatrix[i] != Point(0,0) && circleMatrix[getNeighborInMatrix(i,1)] != Point(0,0)) {horiAndVert[0].push_back(_temp_dist);horiAndVert[2].push_back(_temp_value);horiAndVert[3].push_back(_temp_value2);}
		_temp_dist=distanceOfPoints(circleMatrix[i],circleMatrix[getNeighborInMatrix(i,2)]);
		_temp_value=abs(circleMatrix[i].x-circleMatrix[getNeighborInMatrix(i,2)].x);
		_temp_value2=abs(circleMatrix[i].y-circleMatrix[getNeighborInMatrix(i,2)].y);
		if (_temp_dist != 0 && circleMatrix[i] != Point(0,0) && circleMatrix[getNeighborInMatrix(i,2)] != Point(0,0)) {horiAndVert[1].push_back(_temp_dist);horiAndVert[4].push_back(_temp_value);horiAndVert[5].push_back(_temp_value2);}
		_temp_dist=distanceOfPoints(circleMatrix[i],circleMatrix[getNeighborInMatrix(i,3)]);
		_temp_value=abs(circleMatrix[i].x-circleMatrix[getNeighborInMatrix(i,3)].x);
		_temp_value2=abs(circleMatrix[i].y-circleMatrix[getNeighborInMatrix(i,3)].y);
		if (_temp_dist != 0 && circleMatrix[i] != Point(0,0) && circleMatrix[getNeighborInMatrix(i,3)] != Point(0,0)) {horiAndVert[1].push_back(_temp_dist);horiAndVert[4].push_back(_temp_value);horiAndVert[5].push_back(_temp_value2);}
	}
	//check if all horizontals are nearly the same & get horizontal middle value
	// j=0 is horizontal, j=1 is vertical
	//vector<double> _distanceBetweenPointsInMatrix(2);
	bool nearlySame=true;
	// for distance values and also middle value of x,y for each direction (left/right and above/under)
	for (int j=0;j<6;j++)
	{
		_distanceBetweenPointsInMatrix[j]=0;
		for (int i=0;i<horiAndVert[j].size();i++){_distanceBetweenPointsInMatrix[j]+=horiAndVert[j][i];}
		_distanceBetweenPointsInMatrix[j]/=horiAndVert[j].size();
		// for the distance values check here
		if (j<2)
		{
		for (int i=0;i<horiAndVert[j].size();i++)
		{
			if (nearlySame && !nearlyEqual(_distanceBetweenPointsInMatrix[j],horiAndVert[j][i],60)) {nearlySame=false;}
		}
		}
	}
	if (nearlyEqual(_distanceBetweenPointsInMatrix[0],_distanceBetweenPointsInMatrix[1],40)) {return nearlySame;}
	else return false;
}
bool DetectionCircles::calc3rdOutOf2(int a,int b,int c)
{
bool change=false;
int findOut;
if(circleMatrix[a]==Point(0,0) && circleMatrix[b]!=Point(0,0) && circleMatrix[c]!=Point(0,0))
{
circleMatrix[a].x=circleMatrix[b].x-(circleMatrix[c].x-circleMatrix[b].x );
circleMatrix[a].y=circleMatrix[b].y-(circleMatrix[c].y-circleMatrix[b].y );
change=true;
}
if(circleMatrix[a]!=Point(0,0) && circleMatrix[b]==Point(0,0) && circleMatrix[c]!=Point(0,0))
{
circleMatrix[b].x=circleMatrix[a].x+0.5*(circleMatrix[c].x-circleMatrix[a].x );
circleMatrix[b].y=circleMatrix[a].y+0.5*(circleMatrix[c].y-circleMatrix[a].y );
change=true;
}
if(circleMatrix[a]!=Point(0,0) && circleMatrix[b]!=Point(0,0) && circleMatrix[c]==Point(0,0))
{
circleMatrix[c].x=circleMatrix[b].x+(circleMatrix[b].x-circleMatrix[a].x );
circleMatrix[c].y=circleMatrix[b].y+(circleMatrix[b].y-circleMatrix[a].y );
change=true;
}
return change;
}

bool DetectionCircles::setNeighbors(int cir)
{
	bool change=false;
	if (circleMatrix[cir]!=Point(0,0)) 
	{
		// check if all neighbors have values
		if (circleMatrix[getNeighborInMatrix(cir,0)] == Point(0,0)) 
		{
			circleMatrix[getNeighborInMatrix(cir,0)].x=circleMatrix[cir].x-_distanceBetweenPointsInMatrix[2];
			circleMatrix[getNeighborInMatrix(cir,0)].y=circleMatrix[cir].y-_distanceBetweenPointsInMatrix[3];
			change=true;
		}
		if (circleMatrix[getNeighborInMatrix(cir,1)] == Point(0,0)) 
		{
			circleMatrix[getNeighborInMatrix(cir,1)].x=circleMatrix[cir].x+_distanceBetweenPointsInMatrix[2];
			circleMatrix[getNeighborInMatrix(cir,1)].y=circleMatrix[cir].y+_distanceBetweenPointsInMatrix[3];
			change=true;
		}
		if (circleMatrix[getNeighborInMatrix(cir,2)] == Point(0,0)) 
		{
			circleMatrix[getNeighborInMatrix(cir,2)].x=circleMatrix[cir].x-_distanceBetweenPointsInMatrix[4];
			circleMatrix[getNeighborInMatrix(cir,2)].y=circleMatrix[cir].y-_distanceBetweenPointsInMatrix[5];
			change=true;
		}
		if (circleMatrix[getNeighborInMatrix(cir,3)] == Point(0,0)) 
		{
			circleMatrix[getNeighborInMatrix(cir,3)].x=circleMatrix[cir].x+_distanceBetweenPointsInMatrix[4];
				circleMatrix[getNeighborInMatrix(cir,3)].y=circleMatrix[cir].y+_distanceBetweenPointsInMatrix[5];
			change=true;
		}
	}
	return change;
}

void DetectionCircles::fillCircleMatrix()
{
	// getting corner points
	// set first circle in every corner
	// get xMin and yMin
	int xMin=circles[0].x,yMin=circles[0].y,xMax=circles[0].x,yMax=circles[0].y;;
	for (int i=1;i<circles.size();i++)
	{
		if (circles[i].x < xMin) {xMin=circles[i].x;}
		if (circles[i].y < yMin) {yMin=circles[i].y;}
		if (circles[i].x > xMax) {xMax=circles[i].x;}
		if (circles[i].y > yMax) {yMax=circles[i].y;}
	}
	int xMid=xMin+(xMax-xMin)/2;
	int yMid=yMin+(yMax-yMin)/2;
	// empty previous values
	circleMatrix.clear();
	bool row0=false,row1=false,row2=false,col0=false,col1=false,col2=false;
	for (int i=0;i<circles.size();i++)
	{
		if (nearlyEqual(circles[i].x,xMin,5) && nearlyEqual(circles[i].y,yMin,5)) {circleMatrix[0]=circles[i];row0=true;col0=true;}
		if (nearlyEqual(circles[i].x,xMid,5) && nearlyEqual(circles[i].y,yMin,5)) {circleMatrix[1]=circles[i];row0=true;col1=true;}
		if (nearlyEqual(circles[i].x,xMax,5) && nearlyEqual(circles[i].y,yMin,5)) {circleMatrix[2]=circles[i];row0=true;col2=true;}
		if (nearlyEqual(circles[i].x,xMin,5) && nearlyEqual(circles[i].y,yMid,5)) {circleMatrix[3]=circles[i];row1=true;col0=true;}
		if (nearlyEqual(circles[i].x,xMid,5) && nearlyEqual(circles[i].y,yMid,5)) {circleMatrix[4]=circles[i];row1=true;col1=true;}
		if (nearlyEqual(circles[i].x,xMax,5) && nearlyEqual(circles[i].y,yMid,5)) {circleMatrix[5]=circles[i];row1=true;col2=true;}
		if (nearlyEqual(circles[i].x,xMin,5) && nearlyEqual(circles[i].y,yMax,5)) {circleMatrix[6]=circles[i];row2=true;col0=true;}
		if (nearlyEqual(circles[i].x,xMid,5) && nearlyEqual(circles[i].y,yMax,5)) {circleMatrix[7]=circles[i];row2=true;col1=true;}
		if (nearlyEqual(circles[i].x,xMax,5) && nearlyEqual(circles[i].y,yMax,5)) {circleMatrix[8]=circles[i];row2=true;col2=true;}
	}
	// if 2, add third
	// each row and column must have at least one value, check if we can go on
	if (row0 && row2 && col0 && col1 && col2 || row0 && row1 && row2 && col0 && col2)
	{
		while (calc3rdOutOf2(0,1,2) | calc3rdOutOf2(3,4,5) | calc3rdOutOf2(6,7,8) | calc3rdOutOf2(0,3,6) | calc3rdOutOf2(1,4,7) | calc3rdOutOf2(2,5,8));
		// get basic horizontal/vertical values hortAndVert 0= horizontal, 1=vertical
		bool correctDistancesBetween = calculate_distanceBetweenPointsInMatrixAreNearlySame();
		// only if vertical and horizontal distances between neighbors are the nearly same, we have enough points.
		// to avoid this (:	
		// 0 0 X				0 0 X
		// 0 X X will be set	X 0 X and cannot be calculated (if is movable for 1 col to the left and right)
		// 0 0 X				0 0 X
		//also compare with calibrated value 
		if (correctDistancesBetween && nearlyEqual(_distanceBetweenPoints[0],_distanceBetweenPointsInMatrix[0],40))
		{
			//while (calc3rdOutOf2(0,1,2) | calc3rdOutOf2(3,4,5) | calc3rdOutOf2(6,7,8) | calc3rdOutOf2(0,3,6) | calc3rdOutOf2(1,4,7) | calc3rdOutOf2(2,5,8));
			// if there are still some points missing
			bool change=true;
			while (change)
			{
				for (int cir=0;cir<circleMatrix.size();cir++)
				{
					change=setNeighbors(cir);
				}
			}
		}
		else {circleMatrix.clear();}
	}
	else {circleMatrix.clear();}
	// if circleMatrix was successful created
	if (circleMatrix.size()>0)
	{
		_distanceBetweenPoints[0]=_distanceBetweenPointsInMatrix[0];
		_distanceBetweenPoints[1]=_distanceBetweenPointsInMatrix[1];
	}
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
	threshold(_working_frame, _working_frame,  120, 255, CV_THRESH_BINARY );
	Canny(_working_frame, _working_frame, 255, 255, 3);
	vector<vector<Point>> contours;
	vector<Point> approx;
    findContours( _working_frame, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE );
for (int i = 0; i < contours.size(); i++)
{
    // Approximate contour with accuracy proportional
    // to the contour perimeter
    cv::approxPolyDP(Mat(contours[i]),approx,arcLength(Mat(contours[i]), true) * 0.02,true);

	if (approx.size() > 6)
	{
        // Detect and label circles
        double area = contourArea(contours[i]);
        cv::Rect r = boundingRect(contours[i]);
        int radius = r.width / 2;
        if (abs(1 - ((double)r.width / r.height)) <= 0.2 & abs(1 - (area / (CV_PI * std::pow(radius, 2)))) <= 0.2)
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

double DetectionCircles::distanceOfPoints(Point i,Point j)
{
	return sqrt(pow((i.x-j.x),2)+pow((i.y-j.y),2));
}