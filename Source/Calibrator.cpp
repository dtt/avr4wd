#include "Calibrator.h"

/**
 * @bried    : �L�����u���[�V�����_��ݒ肷�郁�\�b�h
 * @pram[in] : �L�����u���[�V�����p�̓_��
 * @return   : �Ȃ�
 */
void Calibrator::addCalibrationPoint(CvPoint point){
	if(this->inputCalibrationPoints->size()  <  this->CALIBRATION_NUM ){
		this->inputCalibrationPoints->push_back(point);

	}
}

/**
 * @bried    : �^����ꂽ�L�����u���[�V�����_����ˉe�ϊ��s��𐶐����t�B�[���h�ɕۑ����郁�\�b�h
 * @pram[in] : �Ȃ�
 * @return   : �Ȃ�
 */
void Calibrator::createHomoMatrix(){
	// �ϊ��O�ƌ�̉摜�ł̍��W
	CvMat    src_point, dst_point;

	float    a[] = {
		(float)this->inputCalibrationPoints->at(0).x, (float)this->inputCalibrationPoints->at(0).y,
		(float)this->inputCalibrationPoints->at(1).x, (float)this->inputCalibrationPoints->at(1).y,
		(float)this->inputCalibrationPoints->at(2).x, (float)this->inputCalibrationPoints->at(2).y,
		(float)this->inputCalibrationPoints->at(3).x, (float)this->inputCalibrationPoints->at(3).y,

	};
	// ���͉摜�̃L�����u���[�V�����|�C���g�̓o�^
	src_point = cvMat( this->CALIBRATION_NUM, 2, CV_32FC1, a );

	// ���ʉ摜�̃L�����u���[�V�����|�C���g�̓o�^
	float    b[] = {
		(float)0, (float)0,
		(float)0, (float)(this->dstImageSize.height - 1),
		(float)(this->dstImageSize.width - 1), (float)(this->dstImageSize.height - 1),
		(float)(this->dstImageSize.width - 1), (float)0
	};
	dst_point = cvMat( this->CALIBRATION_NUM, 2, CV_32FC1, b );

	// homography �s����쐬
	this->homoMatrix = cvCreateMat( 3, 3, CV_32FC1 );
	// homography �s��̐���
	cvFindHomography( &src_point, &dst_point, this->homoMatrix );
}


/**
 * @bried    : �������ꂽ�ˉe�ϊ��s��Ɋ�Â��āC�����̓_���W�̎ˉe���擾���郁�\�b�h
 * @pram[in] : �_���W
 * @return   : �ˉe�ϊ���̓_���W
 */
CvPoint Calibrator::homographyPointTranslation(CvPoint srcPoint){

	CvMat *pp = cvCreateMat(1, 1, CV_32FC2); // 2�`�����l����1x1�s��
	CvMat *qq = cvCreateMat(1, 1, CV_32FC2);
	cvSet1D(pp, 0, cvScalar(srcPoint.x, srcPoint.y));
	
	cvPerspectiveTransform(pp, qq, this->getHomoMatrix());
	
	CvScalar q_ = cvGet1D(qq, 0);
	CvPoint q = cvPoint(cvRound(q_.val[0]), cvRound(q_.val[1]));
	
	cvReleaseMat(&pp);
	cvReleaseMat(&qq);

	return q;
}


/**
 * @bried    : �������ꂽ�ˉe�ϊ��s��Ɋ�Â��āC�����̉摜�̎ˉe���擾���郁�\�b�h
 * @pram[in] : �摜
 * @return   : �ˉe�ϊ���̉摜
 */
void Calibrator::homographyImageTranslation(IplImage *srcImage, IplImage *dstImage){
	cvWarpPerspective( srcImage, dstImage, this->getHomoMatrix() );
}

