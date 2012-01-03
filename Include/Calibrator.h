#pragma once

#include <vector>
#include "OpenCV.h"

/**
 * �L�����u���[�V�����p�̓_�񂩂�ˉe�ϊ��s��𐶐���
 * �s��Ɋ�Â��ĔC�ӂ̉摜��_���ϊ�����N���X
 * @author   : Junpei TSUJI (DT&T)
 * @version  : 1.0.0-2011.12.25
 */
class Calibrator
{
public:

	/**
	 * @bried    : �R���X�g���N�^
	 * @pram[in] : �Ȃ�
	 */
	Calibrator(){
		this->dstImageSize = cvSize(320, 240);
		this->inputCalibrationPoints = new std::vector<CvPoint>();
		this->homoMatrix = NULL;
	}
	/**
	 * @bried    : �f�X�g���N�^
	 * @pram[in] : �Ȃ�
	 */
	~Calibrator(){
		if( this->inputCalibrationPoints != NULL ) delete this->inputCalibrationPoints;
	}


	/**
	 * @bried    : �s��̉摜�T�C�Y��ݒ肷�郁�\�b�h
	 * @pram[in] : �摜�T�C�Y�̉����C�c��
	 * @return   : �Ȃ�
	 */
	void setDestinationImageSize(int width, int height){
		this->dstImageSize = cvSize(width, height);
	}

	/**
	 * @bried    : �s��̉摜�T�C�Y���擾���郁�\�b�h
	 * @pram[in] : �Ȃ�
	 * @return   : �摜�T�C�Y
	 */
	CvSize getDestinationImageSize(){
		return this->dstImageSize;
	}
 
	/**
	 * @bried    : �L�����u���[�V�����_��ݒ肷�郁�\�b�h
	 * @pram[in] : �L�����u���[�V�����p�̓_��
	 * @return   : �Ȃ�
	 */
	void setCalibrationPoints(std::vector<CvPoint>  *inputCalibrationPoints){
		this->inputCalibrationPoints = inputCalibrationPoints;
	}
	/**
	 * @bried    : �L�����u���[�V�����_���擾���郁�\�b�h
	 * @pram[in] : �Ȃ�
	 * @return   : �L�����u���[�V�����p�̓_��
	 */
	std::vector<CvPoint> *getCalibrationPoints(){
		return this->inputCalibrationPoints;
	}
	/**
	 * @bried    : �L�����u���[�V�����_��ǉ����郁�\�b�h
	 * @pram[in] : �L�����u���[�V�����_
	 * @return   : �Ȃ�
	 */
	void addCalibrationPoint(CvPoint point);


	/**
	 * @bried    : �L�����u���[�V������̎ˉe�ϊ��s���ݒ肷�郁�\�b�h
	 * @pram[in] : �ˉe�ϊ��s��
	 * @return   : �Ȃ�
	 */
	void setHomoMatrix(CvMat *homoMatrix){
		this->homoMatrix = homoMatrix;
	}
	/**
	 * @bried    : �L�����u���[�V������̎ˉe�ϊ��s����擾���郁�\�b�h
	 * @pram[in] : �Ȃ�
	 * @return   : �ˉe�ϊ��s��
	 */
	CvMat *getHomoMatrix(){
		return this->homoMatrix;
	}
	/**
	 * @bried    : �^����ꂽ�L�����u���[�V�����_����ˉe�ϊ��s��𐶐����t�B�[���h�ɕۑ����郁�\�b�h
	 * @pram[in] : �Ȃ�
	 * @return   : �Ȃ�
	 */
	void createHomoMatrix();

	/**
	 * @bried    : �������ꂽ�ˉe�ϊ��s��Ɋ�Â��āC�����̓_���W�̎ˉe���擾���郁�\�b�h
	 * @pram[in] : �_���W
	 * @return   : �ˉe�ϊ���̓_���W
	 */
	CvPoint Calibrator::homographyPointTranslation(CvPoint srcPoint);
	/**
	 * @bried    : �������ꂽ�ˉe�ϊ��s��Ɋ�Â��āC�����̉摜�̎ˉe���擾���郁�\�b�h
	 * @pram[in] : �摜
	 * @return   : �ˉe�ϊ���̉摜
	 */
	void homographyImageTranslation(IplImage *srcImage, IplImage *dstImage);

	/**
	 * @bried    : �L�����u���[�V�����_�̐�
	 */
	static const int CALIBRATION_NUM = 4;	

protected:
	/**
	 * @bried    : �s��̉摜�T�C�Y
	 */
	CvSize dstImageSize;       

	/**
	 * @bried    : �L�����u���[�V�����p��4�_�̔z�� (x0, y0), (x1, y1), ..., (x3, y3)
	 */
	std::vector<CvPoint>  *inputCalibrationPoints;       
    
	/**
	 * @bried    : �ˉe�ϊ��s�� (�z���O���t�B�s��)
	 */
	CvMat *homoMatrix;  

};
