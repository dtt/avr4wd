#include "CameraControl.h"


CameraControl::CameraControl(void)
{
}


CameraControl::~CameraControl(void)
{
}

/*
�J���������������郁�\�b�h
deviceID�Ńf�o�C�X�ԍ����Awidth,height�ł��ꂼ��ǂݍ��މ摜�̕��A������
�w�肷��
*/
int CameraControl::cameraSetup(int deviceID,int width,int height){
	
	if( vi.listDevices() > 0 ){
		vi.setupDevice(deviceID,width,height);//VideoInput Library�ɂ�鏉����
		vi.showSettingsWindow(deviceID);//�p�����[�^�Z�b�g�A�b�v�E�C���h�E��\��
		return 1;
	}
	else{
		// �J�����������Ȃ�
		return 0;
	}
}
/*
�J��������1�t���[�����o�����\�b�h
deviceID�Ńf�o�C�X�ԍ����w��,image�Ńt���[�����i�[����IplImage���w�肷��B
�摜�͏㉺�����]������Ԃœn�����̂�,�㉺���]�ł͂Ȃ���Ԃɂ��������isReverse = true�Ƃ��邱�ƁB
*/
void CameraControl::captureFrameThroughVideoInput(int deviceID, IplImage *image, bool isReverse){
	memcpy(image->imageData, vi.getPixels(deviceID, false), image->imageSize);//�J�������瓾���P�t���[�����R�s�[
	if(isReverse){
		cvFlip(image,0);//�㉺���]����
	}
}
