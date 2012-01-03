#pragma once

#include "OpenCV.h"
#include "videoInput.h"

/**
 * OpenCV�ł̃J�����L���v�`����������VideoInput Library����J�����摜���擾����N���X
 * OpenCV�ł͕s�\�ȃJ�����̃p�����[�^�ݒ���\�ɂ���
 * @author   : Sho YAMAUCHI (DT&T)
 * @version  : 1.0.0-2011.12.25
 */
class CameraControl
{
public:
//--------------------constructor------------------------
	CameraControl(void);
//--------------------destructor-------------------------
	~CameraControl(void);
//----------------------methods--------------------------
	int  cameraSetup(int deviceID,int width,int height);
	void captureFrameThroughVideoInput(int deviceID, IplImage *image, bool isReverse);
//---------------------fields----------------------------
	videoInput vi;
	static const int defaultInputCameraSizeWidth  = 640;
	static const int defaultInputCameraSizeHeight = 480;

};

