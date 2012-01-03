#pragma once

#include <iostream>
#include <stdio.h>
#include <deque>				//デック

#include "OpenCV.h"

#include "Course.h"
#include "Calibrator.h"
#include "CourseEditor.h"
#include "Race.h"


//------------------------Invariants----------------------//
#define THRESHOLD 20			//しきい値
#define THRESHOLD_MAX_VALUE 255	//しきい値の最大値
#define SCALE (1.0/255.0)		//L*a*b表色系に変換するために必要なスケールファクタ


//-------------------------Flags--------------------------//
#define CAPTURE_OFF 0			//画像キャプチャ中断
#define CAPTURE_ON 1			//画像キャプチャ開始
#define COLOR_DIFFERENCE 0		//RGB成分を用いた差分算出
#define LAB_DIFFERENCE 1		//L*a*b表色系を用いた差分算出
#define GRAY_DIFFERENCE 2		//グレースケールを用いた差分算出
#define NOISE_KEEP 0			//ノイズ除去無しのフラグ値
#define NOISE_MORPHOLOGY 1		//モルフォロジー演算を用いたノイズ除去
#define NOISE_MEDIAN 2			//メディアンフィルタを用いたノイズ除去


/**
 * レース情報をカメラによって追跡・管理する中枢クラス
 * @author   : Sho YAMAUCHI, Junpei TSUJI (DT&T)
 * @version  : 1.0.0-2011.12.25
 */
class RaceTrackingObject : public Race
{
public:
	/**
	 * @bried    : コンストラクタ
	 * @pram[in] : コース情報を読み込むファイル名
	 */
	RaceTrackingObject(std::string filename);
	/**
	 * @bried    : デストラクタ
	 * @pram[in] : なし
	 */
	~RaceTrackingObject(void);


	//----------------Methods-------------------//
	double  getDistance(CvPoint point1, CvPoint point2);
	void    colorDifference(IplImage *currentImage,IplImage *backgroundImage,IplImage *resultImage);
	void    labDifference(IplImage *currentImage,IplImage *backgroundImage,IplImage *resultImage);
	void    grayScaleDifference(IplImage *currentImage,IplImage *backgroundImage,IplImage *resultImage);
	
	void    startRaceTracking();

	CvPoint getCurrentPosition(CvSeq* contour,CvPoint previousPosition);
	int     getCurrentCheckPointIndex(CvPoint currentPoint);
	void    setCurrentDirection();

	// regist observers
	bool attachRaceObserver( RaceObserver* ob );
	bool detachRaceObserver( RaceObserver* ob );
	bool attachMachineObserver( MachineObserver* ob );
	bool detachMachineObserver( MachineObserver* ob );

	// notify
	void updateNotify(UpdateEvent *e);
	void outOfBoundNotify(MachineObserver* ob, OutOfBoundEvent *e);
	void controllDataNotifyAll(ControllDataEvent *e);

	//---------------Fields---------------------//
	CvSize                         imageSize;
	Calibrator                     *calibrator;
	Course                         *course;
	CourseEditor                   *editor;
	RaceObserver                   *raceObserver;
	std::vector<MachineObserver *> *machineObservers;

	CvPoint                        currentPoint;
	CvPoint                        previousPoint;
	cv::Point2d                    currentDirection;
	int                            currentCheckPointIndex;
};

