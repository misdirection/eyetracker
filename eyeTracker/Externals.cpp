#pragma once
#include "eyeTracker.h"


Externals::Externals(void)
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

bool Externals::loadFaceCascade()
{
	return face_cascade.load("haarcascade_frontalface_alt2.xml");
}

bool Externals::loadEyeCascade()
{
	return eye_cascade.load("haarcascade_eye.xml");
}


