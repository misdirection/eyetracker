#pragma once
#include "eyeTracker.h"


Externals::Externals(void)
	fs("default.xml", FileStorage::READ),
	storageFace(FileStorage("haarcascade_frontalface_alt2.xml", FileStorage::READ)),
	storageEye(FileStorage("haarcascade_eye.xml", FileStorage::READ))
{

}


Externals::~Externals(void)
{
}



FileNode Externals::getFaceCascade()
{
	return storageFace.getFirstTopLevelNode();
}
FileNode Externals::getEyeCascade()
{
	return storageEye.getFirstTopLevelNode();
}
FileNode Externals::getFaceCascade()
{
	return storageFace.getFirstTopLevelNode();
}
FileNode Externals::getEyeCascade()
{
	return storageEye.getFirstTopLevelNode();
}

FileStorage Externals::getCalibFile()
{
	return fs;
}
bool Externals::loadFaceCascade()
{
return face_cascade.read(storageFace.getFirstTopLevelNode());
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
bool Externals::loadEyeCascade()
{
	return eye_cascade.read(storageEye.getFirstTopLevelNode());
}
