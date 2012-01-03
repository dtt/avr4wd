#include "CameraControl.h"


CameraControl::CameraControl(void)
{
}


CameraControl::~CameraControl(void)
{
}

/*
カメラを初期化するメソッド
deviceIDでデバイス番号を、width,heightでそれぞれ読み込む画像の幅、高さを
指定する
*/
int CameraControl::cameraSetup(int deviceID,int width,int height){
	
	if( vi.listDevices() > 0 ){
		vi.setupDevice(deviceID,width,height);//VideoInput Libraryによる初期化
		vi.showSettingsWindow(deviceID);//パラメータセットアップウインドウを表示
		return 1;
	}
	else{
		// カメラが一台もない
		return 0;
	}
}
/*
カメラから1フレーム取り出すメソッド
deviceIDでデバイス番号を指定,imageでフレームを格納するIplImageを指定する。
画像は上下が反転した状態で渡されるので,上下反転ではない状態にしたければisReverse = trueとすること。
*/
void CameraControl::captureFrameThroughVideoInput(int deviceID, IplImage *image, bool isReverse){
	memcpy(image->imageData, vi.getPixels(deviceID, false), image->imageSize);//カメラから得た１フレームをコピー
	if(isReverse){
		cvFlip(image,0);//上下反転処理
	}
}
