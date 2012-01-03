#pragma once

#include "OpenCV.h"
#include "videoInput.h"

/**
 * OpenCVでのカメラキャプチャをせずにVideoInput Libraryからカメラ画像を取得するクラス
 * OpenCVでは不可能なカメラのパラメータ設定を可能にする
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

