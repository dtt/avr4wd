#pragma once

#include <iostream>
#include <stdio.h>
#include <vector>

#include "OpenCV.h"
#include "Calibrator.h"
#include "Course.h"

/**
 * マシンが走るコースを生成するメソッド
 * コースデータをXML形式でファイル出力したり, ファイルから入力できるメソッド
 * @author   : Junpei TSUJI (DT&T)
 * @version  : 1.0.0-2011.12.25
 */
class CourseEditor
{
public:
	/**
	 * @bried    : コンストラクタ
	 * @pram[in] : キャリブレーションクラスのインスタンスとコースクラスのインスタンス
	 */ 
	CourseEditor(Calibrator *calibrator, Course *course);
	/**
	 * @bried    : コンストラクタ (XMLファイルよりキャリブレーションデータとコースデータを取得する)
	 * @pram[in] : キャリブレーションクラスのインスタンスとコースクラスのインスタンス, データを取得するファイル名
	 */ 
	CourseEditor(Calibrator *calibrator, Course *course, std::string filename);
	/**
	 * @bried    : デストラクタ
	 * @pram[in] : なし
	 */
	~CourseEditor(void);

	/**
	 * @bried    : XMLファイルからキャリブレーションデータとコースデータを読み込んでインスタンスを生成し, 引数のポインタに代入するメソッド
	 * @pram[in] : キャリブレーションクラスのインスタンスとコースクラスのインスタンス, データを取得するファイル名
	 * @return   : なし
	 */
	void readFile(Calibrator *calibrator, Course *course, std::string filename);
	/**
	 * @bried    : キャリブレーションデータとコースデータをXMLに書き出し
	 * @pram[in] : キャリブレーションクラスのインスタンスとコースクラスのインスタンス, データを掃き出すファイル名
	 * @return   : なし
	 */	
	void writeFile(Calibrator *calibrator, Course *course, std::string filename);


	/**
	 * @bried    : 画像にコース点を描画する
	 * @pram[in] : コースを描画する元画像のポインタとコースクラスのポインタ
	 * @return   : なし
	 */ 
	void drawCoursePoints(IplImage *image, Course *course);
	/**
	 * @bried    : 画像にコースの線を描画する
	 * @pram[in] : コースを描画する元画像のポインタとコースクラスのポインタ
	 * @return   : なし
	 */ 
	void drawCourseLines(IplImage *image, Course *course);
	/**
	 * @bried    : 画像にキャリブレーション点を描画する
	 * @pram[in] : 描画する元画像のポインタとキャリブレーションクラスのポインタ
	 * @return   : なし
	 */ 
	void drawCalibrationPoints(IplImage *image, Calibrator *calibrator);

	/**
	 * @bried    : キャリブレーションに関するマウスクリックイベントハンドラ
	 * @pram[in] : マウスクリックイベントの入力値
	 * @return   : なし
	 */
	static void cameraCalibrationEventHandler( int event, int x, int y, int flags ,void *calibratorInstance);
	/**
	 * @bried    : コース作成関するマウスクリックイベントハンドラ
	 * @pram[in] : マウスクリックイベントの入力値
	 * @return   : なし
	 */
	static void createCourseEventHandler( int event, int x, int y, int flags ,void *courseInstance);

};


