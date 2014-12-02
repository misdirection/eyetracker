#pragma once
#include "eyeTracker.h"
#include "fps.h"
#include "ThreadCapturing.h"
#include "ThreadData.h"
#include "calib.h"
#include <fstream>

VideoCapture* captureDevice = new VideoCapture;
// data thread
ThreadData *datas = new ThreadData;
// capture threads in a map for easy access
map<int,ThreadCapturing*> devices;
Externals* files = new Externals;
DeviceInformation* captureDeviceInfo = new DeviceInformation;

//counts attached devices and returns the amount of devices
bool setup()
{
	// attached devices are checked and device informations will be available
	// check devices from 0 to x. If available, then add value to the vector with null pointer
	int number=0;
	while (number>=0)
	{
		captureDevice->open(number);
		if (!captureDevice->isOpened())
		{
			//no device -> return unsuccessful setup;
			//when all devices recognized, go out of while loop
			if (number == 0) {cout << "no available devices found\n"; return false;}
			number=-1;
		}
		else
		{
			//test cout for the path and name of the device (accessable everywhere due to extern)
			// use this to check if there is a calib file already
			//cout << captureDeviceInfo->getName(number) << endl;			
			//cout << captureDeviceInfo->getPath(number) << endl;			
			devices[number]=nullptr;
			number++;
			captureDevice->release();
		}
	}
	// loading cascade files
	// allocating space for the Externals class
	if (!files->loadFaceCascade()) {cout << "no haar cascade file for face recognition found\n"; return false;}
	if (!files->loadEyeCascade()) {cout << "no haar cascade file for eye recognition found\n"; return false;}
	if (!files->loadCalibFile()){cout << "Could not open the configuration file: \"" << "default.xml" << "\"" << endl; return false;
	}
	return true;
}

void switchDevices(int number)
{
	if (number<devices.size())
	{
		if (devices[number]==nullptr)
		{
			stringstream text;
			text<< "web cam device " << number; 
			string str1= text.str();
			devices[number]=new ThreadCapturing(const_cast<char *>(str1.c_str()),*captureDevice, number,datas->getThreadId());
			devices[number]->Start();
		}
		else
		{
			devices[number]->Stop();
			delete devices[number];
			devices[number]=nullptr;
		}
	}
}

int run(int j)
{
	Settings s;
	// Read the settings
	files->getCalibFile()["Settings"] >> s;
	files->getCalibFile().release();                                         // close Settings file

	if (!s.goodInput)
	{
		cout << "Invalid input detected. Application stopping. " << endl;
		//return -1;
	}

	vector<vector<Point2f> > imagePoints;
	Mat cameraMatrix, distCoeffs;
	Size imageSize;
	int mode = s.inputType == Settings::IMAGE_LIST ? CAPTURING : DETECTION;
	clock_t prevTimestamp = 0;
	const Scalar RED(0,0,255), GREEN(0,255,0);
	const char ESC_KEY = 27;

	for(int i = 0;;++i)
	{
		Mat view;
		bool blinkOutput = false;

		view = s.nextImage();

		//-----  If no more image, or got enough, then stop calibration and show result -------------
		if( mode == CAPTURING && imagePoints.size() >= (unsigned)s.nrFrames )
		{
			if( runCalibrationAndSave(s, imageSize,  cameraMatrix, distCoeffs, imagePoints,j))
				mode = CALIBRATED;
			else
				mode = DETECTION;
		}
		if(view.empty())          // If no more images then run calibration, save and stop loop.
		{
			if( imagePoints.size() > 0 )
				runCalibrationAndSave(s, imageSize,  cameraMatrix, distCoeffs, imagePoints,j);
			break;
		}


		imageSize = view.size();  // Format input image.
		if( s.flipVertical )    flip( view, view, 0 );

		vector<Point2f> pointBuf;

		bool found;
		switch( s.calibrationPattern ) // Find feature points on the input format
		{
		case Settings::CHESSBOARD:
			found = findChessboardCorners( view, s.boardSize, pointBuf,
				CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
			break;
		case Settings::CIRCLES_GRID:
			found = findCirclesGrid( view, s.boardSize, pointBuf );
			break;
		case Settings::ASYMMETRIC_CIRCLES_GRID:
			found = findCirclesGrid( view, s.boardSize, pointBuf, CALIB_CB_ASYMMETRIC_GRID );
			break;
		default:
			found = false;
			break;
		}

		if ( found)                // If done with success,
		{
			// improve the found corners' coordinate accuracy for chessboard
			if( s.calibrationPattern == Settings::CHESSBOARD)
			{
				Mat viewGray;
				cvtColor(view, viewGray, COLOR_BGR2GRAY);
				cornerSubPix( viewGray, pointBuf, Size(11,11),
					Size(-1,-1), TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));
			}

			if( mode == CAPTURING &&  // For camera only take new samples after delay time
				(!s.inputCapture.isOpened() || clock() - prevTimestamp > s.delay*1e-3*CLOCKS_PER_SEC) )
			{
				imagePoints.push_back(pointBuf);
				prevTimestamp = clock();
				blinkOutput = s.inputCapture.isOpened();
			}

			// Draw the corners.
			drawChessboardCorners( view, s.boardSize, Mat(pointBuf), found );
		}

		//----------------------------- Output Text ------------------------------------------------
		string msg = (mode == CAPTURING) ? "100/100" :
			mode == CALIBRATED ? "Calibrated" : "Press 'g' to start";
		int baseLine = 0;
		Size textSize = getTextSize(msg, 1, 1, 1, &baseLine);
		Point textOrigin(view.cols - 2*textSize.width - 10, view.rows - 2*baseLine - 10);

		if( mode == CAPTURING )
		{
			if(s.showUndistorsed)
				msg = format( "%d/%d Undist", (int)imagePoints.size(), s.nrFrames );
			else
				msg = format( "%d/%d", (int)imagePoints.size(), s.nrFrames );
		}

		putText( view, msg, textOrigin, 1, 1, mode == CALIBRATED ?  GREEN : RED);

		if( blinkOutput )
			bitwise_not(view, view);

		//------------------------- Video capture  output  undistorted ------------------------------
		if( mode == CALIBRATED && s.showUndistorsed )
		{
			Mat temp = view.clone();
			undistort(temp, view, cameraMatrix, distCoeffs);
		}

		//------------------------------ Show image and check for input commands -------------------
		imshow("Image View", view);
		char key = (char)waitKey(s.inputCapture.isOpened() ? 50 : s.delay);

		if( key  == ESC_KEY )
			break;

		if( key == 'u' && mode == CALIBRATED )
			s.showUndistorsed = !s.showUndistorsed;

		if( s.inputCapture.isOpened() && key == 'g' )
		{
			mode = CAPTURING;
			imagePoints.clear();
		}
	}

	// -----------------------Show the undistorted image for the image list ------------------------
	if( s.inputType == Settings::IMAGE_LIST && s.showUndistorsed )
	{
		Mat view, rview, map1, map2;
		initUndistortRectifyMap(cameraMatrix, distCoeffs, Mat(),
			getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, imageSize, 1, imageSize, 0),
			imageSize, CV_16SC2, map1, map2);

		for(int i = 0; i < (int)s.imageList.size(); i++ )
		{
			view = imread(s.imageList[i], 1);
			if(view.empty())
				continue;
			remap(view, rview, map1, map2, INTER_LINEAR);
			//imshow("Image View", rview);
			char c = (char)waitKey();
			if( c  == ESC_KEY || c == 'q' || c == 'Q' )
				break;
		}
	}
	s.inputCapture.release();
	files->getFocalLengths().push_back(cameraMatrix.at<double>(0,0));

	return -1;

}



int main( int argc, const char** argv ) 
{ 
	// get count of recognized cameras 
	cout << "Welcome to the Eyetracker9000!\n-----------------------------------------------------------------" << endl;
	if (!setup()) {cout << "execution will be stopped due to errors.\n";system("pause");return -1;}
	if (devices.size() > 0) 
	{
		cout << "Press 1 to start the calibration for all available devices." << endl;
		cout << "Press 2 to skip calibration and start eyetracking." << endl;
		cout << "Enter number: ";
		int option =0;
		cin >> option;
		cout << "-----------------------------------------------------------------" << endl;
		if(option == 1)
		{
			cout << "Starting calibration for all available devices ("<<devices.size() << ")..."<< endl;
			//Callibration
			for(int i =0; i <devices.size();i++)
			{
				ifstream input;
				string str;
				input.open(captureDeviceInfo->getName(i)+".txt");
				if(!input.is_open())
				{	
					cout << "Can't find an existing file, calibration starts for " << captureDeviceInfo->getName(i) <<"!" <<  endl;
					run(i);
					cvDestroyWindow("Image View");
				}
				else
				{
					double focalLength;
					cout << "loading calibrationfile for " << captureDeviceInfo->getName(i) <<"!" <<  endl;
					getline(input, str);
					focalLength =stod(str);
					files->setFocalLengths(focalLength);
					//cout << "Focal Length"<<focalLength<<endl;

				}

			}
			cout << "Calibration of all devices done!\n-----------------------------------------------------------------" << endl;
			option =2;
		}
		if(option==2)
		{
			cout << "Starting the eyetracker...." << endl;
			//Eyetracking
			cout << "Available devices: " << devices.size() << endl << "Press number between 0 and " << devices.size()-1 << " to start first camera.";
			// until you select an available device
			int number=-1;
			while(number<0 || number >=devices.size() )
			{
				while(!_kbhit());
				string temp; temp= getch(); number = atoi(temp.c_str());
				if (number == 0 && !(temp=="0")) {number=-1;}
			}
			// activate the selected device
			switchDevices(number);
		}
		char key;

		while((key = cvWaitKey(0)) != 0x1b)
		{

			switch(key)
			{
			case '0': switchDevices(0); break;
			case '1': switchDevices(1); break;
			case '2': switchDevices(2); break;
			case '3': switchDevices(3); break;
			case '4': switchDevices(4); break;
			case '5': switchDevices(5); break;
			}
		}
	}
	return (0);
}