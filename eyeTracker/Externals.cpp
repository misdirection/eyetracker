#pragma once
#include "eyeTracker.h"


Externals::Externals(void):
	fs("default.xml", FileStorage::READ)
{

}


Externals::~Externals(void)
{
}

CascadeClassifier Externals::getFaceCascade()
{
	return face_cascade;
}
CascadeClassifier Externals::getEyeCascade()
{
	return eye_cascade;
}

FileStorage Externals::getCalibFile()
{
	return fs;
}


bool Externals::loadFaceCascade()
{
	return face_cascade.load("haarcascade_frontalface_alt2.xml");
}

bool Externals::loadEyeCascade()
{
	return eye_cascade.load("haarcascade_eye.xml");
}

bool Externals::loadCalibFile(){

	if(fs.isOpened()){return true;}else{return false;}
}


void Externals::setFocalLengths(double d){
	files->focalLengths.push_back(d);
};

vector<double> Externals::getFocalLengths()
{
	return focalLengths;

}