#include "Calibrator.h"

/**
 * @bried    : キャリブレーション点を設定するメソッド
 * @pram[in] : キャリブレーション用の点列
 * @return   : なし
 */
void Calibrator::addCalibrationPoint(CvPoint point){
	if(this->inputCalibrationPoints->size()  <  this->CALIBRATION_NUM ){
		this->inputCalibrationPoints->push_back(point);

	}
}

/**
 * @bried    : 与えられたキャリブレーション点から射影変換行列を生成しフィールドに保存するメソッド
 * @pram[in] : なし
 * @return   : なし
 */
void Calibrator::createHomoMatrix(){
	// 変換前と後の画像での座標
	CvMat    src_point, dst_point;

	float    a[] = {
		(float)this->inputCalibrationPoints->at(0).x, (float)this->inputCalibrationPoints->at(0).y,
		(float)this->inputCalibrationPoints->at(1).x, (float)this->inputCalibrationPoints->at(1).y,
		(float)this->inputCalibrationPoints->at(2).x, (float)this->inputCalibrationPoints->at(2).y,
		(float)this->inputCalibrationPoints->at(3).x, (float)this->inputCalibrationPoints->at(3).y,

	};
	// 入力画像のキャリブレーションポイントの登録
	src_point = cvMat( this->CALIBRATION_NUM, 2, CV_32FC1, a );

	// 結果画像のキャリブレーションポイントの登録
	float    b[] = {
		(float)0, (float)0,
		(float)0, (float)(this->dstImageSize.height - 1),
		(float)(this->dstImageSize.width - 1), (float)(this->dstImageSize.height - 1),
		(float)(this->dstImageSize.width - 1), (float)0
	};
	dst_point = cvMat( this->CALIBRATION_NUM, 2, CV_32FC1, b );

	// homography 行列を作成
	this->homoMatrix = cvCreateMat( 3, 3, CV_32FC1 );
	// homography 行列の生成
	cvFindHomography( &src_point, &dst_point, this->homoMatrix );
}


/**
 * @bried    : 生成された射影変換行列に基づいて，引数の点座標の射影を取得するメソッド
 * @pram[in] : 点座標
 * @return   : 射影変換後の点座標
 */
CvPoint Calibrator::homographyPointTranslation(CvPoint srcPoint){

	CvMat *pp = cvCreateMat(1, 1, CV_32FC2); // 2チャンネルの1x1行列
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
 * @bried    : 生成された射影変換行列に基づいて，引数の画像の射影を取得するメソッド
 * @pram[in] : 画像
 * @return   : 射影変換後の画像
 */
void Calibrator::homographyImageTranslation(IplImage *srcImage, IplImage *dstImage){
	cvWarpPerspective( srcImage, dstImage, this->getHomoMatrix() );
}

