#pragma once
#include "eyeTracker.h"


Externals::Externals(void) :	storageFace(FileStorage("haarcascade_frontalface_alt2.xml", FileStorage::READ)),
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
bool Externals::loadFaceCascade()
{
	return face_cascade.read(storageFace.getFirstTopLevelNode());
}

bool Externals::loadEyeCascade()
{
	return eye_cascade.read(storageEye.getFirstTopLevelNode());
}

