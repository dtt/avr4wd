#pragma once

#include <vector>
#include "OpenCV.h"

/**
 * キャリブレーション用の点列から射影変換行列を生成し
 * 行列に基づいて任意の画像や点列を変換するクラス
 * @author   : Junpei TSUJI (DT&T)
 * @version  : 1.0.0-2011.12.25
 */
class Calibrator
{
public:

	/**
	 * @bried    : コンストラクタ
	 * @pram[in] : なし
	 */
	Calibrator(){
		this->dstImageSize = cvSize(320, 240);
		this->inputCalibrationPoints = new std::vector<CvPoint>();
		this->homoMatrix = NULL;
	}
	/**
	 * @bried    : デストラクタ
	 * @pram[in] : なし
	 */
	~Calibrator(){
		if( this->inputCalibrationPoints != NULL ) delete this->inputCalibrationPoints;
	}


	/**
	 * @bried    : 行先の画像サイズを設定するメソッド
	 * @pram[in] : 画像サイズの横幅，縦幅
	 * @return   : なし
	 */
	void setDestinationImageSize(int width, int height){
		this->dstImageSize = cvSize(width, height);
	}

	/**
	 * @bried    : 行先の画像サイズを取得するメソッド
	 * @pram[in] : なし
	 * @return   : 画像サイズ
	 */
	CvSize getDestinationImageSize(){
		return this->dstImageSize;
	}
 
	/**
	 * @bried    : キャリブレーション点を設定するメソッド
	 * @pram[in] : キャリブレーション用の点列
	 * @return   : なし
	 */
	void setCalibrationPoints(std::vector<CvPoint>  *inputCalibrationPoints){
		this->inputCalibrationPoints = inputCalibrationPoints;
	}
	/**
	 * @bried    : キャリブレーション点を取得するメソッド
	 * @pram[in] : なし
	 * @return   : キャリブレーション用の点列
	 */
	std::vector<CvPoint> *getCalibrationPoints(){
		return this->inputCalibrationPoints;
	}
	/**
	 * @bried    : キャリブレーション点を追加するメソッド
	 * @pram[in] : キャリブレーション点
	 * @return   : なし
	 */
	void addCalibrationPoint(CvPoint point);


	/**
	 * @bried    : キャリブレーション後の射影変換行列を設定するメソッド
	 * @pram[in] : 射影変換行列
	 * @return   : なし
	 */
	void setHomoMatrix(CvMat *homoMatrix){
		this->homoMatrix = homoMatrix;
	}
	/**
	 * @bried    : キャリブレーション後の射影変換行列を取得するメソッド
	 * @pram[in] : なし
	 * @return   : 射影変換行列
	 */
	CvMat *getHomoMatrix(){
		return this->homoMatrix;
	}
	/**
	 * @bried    : 与えられたキャリブレーション点から射影変換行列を生成しフィールドに保存するメソッド
	 * @pram[in] : なし
	 * @return   : なし
	 */
	void createHomoMatrix();

	/**
	 * @bried    : 生成された射影変換行列に基づいて，引数の点座標の射影を取得するメソッド
	 * @pram[in] : 点座標
	 * @return   : 射影変換後の点座標
	 */
	CvPoint Calibrator::homographyPointTranslation(CvPoint srcPoint);
	/**
	 * @bried    : 生成された射影変換行列に基づいて，引数の画像の射影を取得するメソッド
	 * @pram[in] : 画像
	 * @return   : 射影変換後の画像
	 */
	void homographyImageTranslation(IplImage *srcImage, IplImage *dstImage);

	/**
	 * @bried    : キャリブレーション点の数
	 */
	static const int CALIBRATION_NUM = 4;	

protected:
	/**
	 * @bried    : 行先の画像サイズ
	 */
	CvSize dstImageSize;       

	/**
	 * @bried    : キャリブレーション用の4点の配列 (x0, y0), (x1, y1), ..., (x3, y3)
	 */
	std::vector<CvPoint>  *inputCalibrationPoints;       
    
	/**
	 * @bried    : 射影変換行列 (ホモグラフィ行列)
	 */
	CvMat *homoMatrix;  

};
