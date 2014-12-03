# define _CRT_SECURE_NO_WARNINGS
#pragma once
#include <iostream>
#include <fstream>
#include "eyeTracker.h"
#include "calib.h"
//#include "eyeTracker.cpp"

DeviceInformation* noOfDev = new DeviceInformation;

Settings::Settings()
{
}
Settings::~Settings()
{
}

void  Settings::write(FileStorage& fs) const                        //Write serialization for this class
{

	fs << "{" << "BoardSize_Width"  << boardSize.width
		<< "BoardSize_Height" << boardSize.height
		<< "Square_Size"         << squareSize
		<< "Calibrate_Pattern" << patternToUse
		<< "Calibrate_NrOfFrameToUse" << nrFrames
		<< "Calibrate_FixAspectRatio" << aspectRatio
		<< "Calibrate_AssumeZeroTangentialDistortion" << calibZeroTangentDist
		<< "Calibrate_FixPrincipalPointAtTheCenter" << calibFixPrincipalPoint

		<< "Write_DetectedFeaturePoints" << bwritePoints
		<< "Write_extrinsicParameters"   << bwriteExtrinsics
		<< "Write_outputFileName"  << outputFileName

		<< "Show_UndistortedImage" << showUndistorsed

		<< "Input_FlipAroundHorizontalAxis" << flipVertical
		<< "Input_Delay" << delay
		<< "Input" << input
		<< "}";
}
void Settings::read(const FileNode& node)                          //Read serialization for this class
{
	node["BoardSize_Width" ] >> boardSize.width;
	node["BoardSize_Height"] >> boardSize.height;
	node["Calibrate_Pattern"] >> patternToUse;
	node["Square_Size"]  >> squareSize;
	node["Calibrate_NrOfFrameToUse"] >> nrFrames;
	node["Calibrate_FixAspectRatio"] >> aspectRatio;
	node["Write_DetectedFeaturePoints"] >> bwritePoints;
	node["Write_extrinsicParameters"] >> bwriteExtrinsics;

	node["Write_outputFileName"] >> outputFileName;
	node["Calibrate_AssumeZeroTangentialDistortion"] >> calibZeroTangentDist;
	node["Calibrate_FixPrincipalPointAtTheCenter"] >> calibFixPrincipalPoint;
	node["Input_FlipAroundHorizontalAxis"] >> flipVertical;
	node["Show_UndistortedImage"] >> showUndistorsed;
	node["Input"] >> input;
	node["Input_Delay"] >> delay;

	interprate();
}
void Settings::interprate()
{
	goodInput = true;
	if (boardSize.width <= 0 || boardSize.height <= 0)
	{
		cerr << "Invalid Board size: " << boardSize.width << " " << boardSize.height << endl;
		goodInput = false;
	}
	if (squareSize <= 10e-6)
	{
		cerr << "Invalid square size " << squareSize << endl;

		goodInput = false;
	}
	if (nrFrames <= 0)
	{
		cerr << "Invalid number of frames " << nrFrames << endl;
		goodInput = false;
	}

	if (input.empty())      // Check for valid input
		inputType = INVALID;
	else
	{
		if (input[0] >= '0' && input[0] <= '9')
		{
			//cout<<"1st"<<input<<endl;
			stringstream ss(input);
			ss >> cameraID;
			inputType = CAMERA;
			cout<<"Device Name"<< endl;
			cout << noOfDev->getPath(cameraID)<<endl;
			//outputFileName=	noOfDev->getPath(cameraID);

		}
		else
		{
			if (readStringList(input, imageList))
			{
				inputType = IMAGE_LIST;
				nrFrames = (nrFrames < (int)imageList.size()) ? nrFrames : (int)imageList.size();
			}
			else
				inputType = VIDEO_FILE;
		}
		if (inputType == CAMERA){
			/*int number=-1;
			while(number<0 || number >=2 )
			{
			while(!_kbhit());
			string temp; temp= getch(); number = atoi(temp.c_str());
			if (number == 0 && !(temp=="0")) {number=-1;}
			}*/
			inputCapture.open(cameraID);

		}
		if (inputType == VIDEO_FILE)
			inputCapture.open(input);
		if (inputType != IMAGE_LIST && !inputCapture.isOpened())
			inputType = INVALID;
	}
	if (inputType == INVALID)
	{
		cerr << " Inexistent input: " << input;
		goodInput = false;
	}

	flag = 0;
	if(calibFixPrincipalPoint) flag |= CV_CALIB_FIX_PRINCIPAL_POINT;
	if(calibZeroTangentDist)   flag |= CV_CALIB_ZERO_TANGENT_DIST;
	if(aspectRatio)            flag |= CV_CALIB_FIX_ASPECT_RATIO;


	calibrationPattern = NOT_EXISTING;
	if (!patternToUse.compare("CHESSBOARD")) calibrationPattern = CHESSBOARD;
	if (!patternToUse.compare("CIRCLES_GRID")) calibrationPattern = CIRCLES_GRID;
	if (!patternToUse.compare("ASYMMETRIC_CIRCLES_GRID")) calibrationPattern = ASYMMETRIC_CIRCLES_GRID;
	if (calibrationPattern == NOT_EXISTING)
	{
		cerr << " Inexistent camera calibration mode: " << patternToUse << endl;
		goodInput = false;
	}
	atImageList = 0;

}
Mat Settings::nextImage()
{
	Mat result;
	if( inputCapture.isOpened() )
	{
		Mat view0;
		inputCapture >> view0;
		view0.copyTo(result);
	}
	else if( atImageList < (int)imageList.size() )
		result = imread(imageList[atImageList++], CV_LOAD_IMAGE_COLOR);

	return result;
}

bool Settings::readStringList( const string& filename, vector<string>& l )
{
	l.clear();
	FileStorage fs(filename, FileStorage::READ);
	if( !fs.isOpened() )
		return false;
	FileNode n = fs.getFirstTopLevelNode();
	if( n.type() != FileNode::SEQ )
		return false;
	FileNodeIterator it = n.begin(), it_end = n.end();
	for( ; it != it_end; ++it )
		l.push_back((string)*it);
	return true;
}



double computeReprojectionErrors( const vector<vector<Point3f> >& objectPoints,
								 const vector<vector<Point2f> >& imagePoints,
								 const vector<Mat>& rvecs, const vector<Mat>& tvecs,
								 const Mat& cameraMatrix , const Mat& distCoeffs,
								 vector<float>& perViewErrors)
{
	vector<Point2f> imagePoints2;
	int i, totalPoints = 0;
	double totalErr = 0, err;
	perViewErrors.resize(objectPoints.size());

	for( i = 0; i < (int)objectPoints.size(); ++i )
	{
		projectPoints( Mat(objectPoints[i]), rvecs[i], tvecs[i], cameraMatrix,
			distCoeffs, imagePoints2);
		err = norm(Mat(imagePoints[i]), Mat(imagePoints2), CV_L2);

		int n = (int)objectPoints[i].size();
		perViewErrors[i] = (float) std::sqrt(err*err/n);
		totalErr        += err*err;
		totalPoints     += n;
	}

	return std::sqrt(totalErr/totalPoints);
}

void calcBoardCornerPositions(Size boardSize, float squareSize, vector<Point3f>& corners,
							  Settings::Pattern patternType /*= Settings::CHESSBOARD*/)
{
	corners.clear();

	switch(patternType)
	{
	case Settings::CHESSBOARD:
	case Settings::CIRCLES_GRID:
		for( int i = 0; i < boardSize.height; ++i )
			for( int j = 0; j < boardSize.width; ++j )
				corners.push_back(Point3f(float( j*squareSize ), float( i*squareSize ), 0));
		break;

	case Settings::ASYMMETRIC_CIRCLES_GRID:
		for( int i = 0; i < boardSize.height; i++ )
			for( int j = 0; j < boardSize.width; j++ )
				corners.push_back(Point3f(float((2*j + i % 2)*squareSize), float(i*squareSize), 0));
		break;
	default:
		break;
	}
}

bool runCalibration( Settings& s, Size& imageSize, Mat& cameraMatrix, Mat& distCoeffs,
					vector<vector<Point2f> > imagePoints, vector<Mat>& rvecs, vector<Mat>& tvecs,
					vector<float>& reprojErrs,  double& totalAvgErr)
{

	cameraMatrix = Mat::eye(3, 3, CV_64F);
	if( s.flag & CV_CALIB_FIX_ASPECT_RATIO )
		cameraMatrix.at<double>(0,0) = 1.0;

	distCoeffs = Mat::zeros(8, 1, CV_64F);

	vector<vector<Point3f> > objectPoints(1);
	calcBoardCornerPositions(s.boardSize, s.squareSize, objectPoints[0], s.calibrationPattern);

	objectPoints.resize(imagePoints.size(),objectPoints[0]);

	//Find intrinsic and extrinsic camera parameters
	double rms = calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix,
		distCoeffs, rvecs, tvecs, s.flag|CV_CALIB_FIX_K4|CV_CALIB_FIX_K5);

	//cout << "Re-projection error reported by calibrateCamera: "<< rms << endl;

	bool ok = checkRange(cameraMatrix) && checkRange(distCoeffs);

	totalAvgErr = computeReprojectionErrors(objectPoints, imagePoints,
		rvecs, tvecs, cameraMatrix, distCoeffs, reprojErrs);

	return ok;
}

// Print camera parameters to the output file
void saveCameraParams(Mat& cameraMatrix, int i )
{
	ofstream myfile;
	myfile.open (noOfDev->getName(i)+".txt");
	myfile << cameraMatrix.at<double>(0,0);
	myfile.close();
}

bool runCalibrationAndSave(Settings& s, Size imageSize, Mat&  cameraMatrix, Mat& distCoeffs,vector<vector<Point2f> > imagePoints, int i )
{
	vector<Mat> rvecs, tvecs;
	vector<float> reprojErrs;
	double totalAvgErr = 0;

	bool ok = runCalibration(s,imageSize, cameraMatrix, distCoeffs, imagePoints, rvecs, tvecs,
		reprojErrs, totalAvgErr);
	cout << (ok ? "Calibration succeeded" : "Calibration failed");
	//	<< ". avg re projection error = "  << totalAvgErr ;

	if( ok )
	{saveCameraParams(cameraMatrix, i);
	destroyAllWindows();
	return ok;

	}
}

