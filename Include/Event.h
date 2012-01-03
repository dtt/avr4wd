#pragma once
#include "Course.h"

/**
 * トラッキングオブジェクト (マシン) の位置情報を
 * RaceObserver を実装したレース描画クラスに通知するためのイベントオブジェクトクラス
 * @author   : Junpei TSUJI (DT&T)
 * @version  : 1.0.0-2011.12.25
 */
class UpdateEvent
{
public:
	// コンストラクタ
	UpdateEvent(){ controllKey = 0; }
	// デストラクタ
	~UpdateEvent(){}

	// イベント発生時刻
	tm        *time;
	// 現在のコース画像
	IplImage  *image;
	// コースクラスのインスタンス
	Course    *course;

	// 現在マシンが通過中のチェックポイントのインデックスを, 各マシンに対してまとめた配列
	std::vector<int>         *currentCheckPointIndexArray;
	// 現在マシンの座標を, 各マシンに対してまとめた配列
	std::vector<CvPoint>     *currentPositionArray;
	// 現在マシンの進行方向のベクトルを, 各マシンに対してまとめた配列
	std::vector<cv::Point2d> *currentDirectionArray;

	// キーボード入力の制御キー (イベント発生元に対して通知する用)
	int controllKey;
};


/**
 * トラッキングオブジェクト (マシン) の境界通過を
 * MachineObserverを実装したマシン制御用クラスに通知するためのイベントオブジェクトクラス
 * @author   : Junpei TSUJI (DT&T)
 * @version  : 1.0.0-2011.12.25
 */
class OutOfBoundEvent
{
public:
	// コンストラクタ
	OutOfBoundEvent(){}
	// デストラクタ
	~OutOfBoundEvent(){}

	// 境界に入ったことを表す定数
	static const int OUT_OF_BOUND_FLAG_IN = 1; 
	// 境界から出たことを表す定数
	static const int OUT_OF_BOUND_FLAG_OUT = -1; 
	// 境界の入出に関する情報が不明の場合を表す定数
	static const int OUT_OF_BOUND_FLAG_UNKNOWN = 0; 

	// 現在通過した境界が「北側」であることを表す定数
	static const int CURRENT_BOUND_EDHE_NORTH = 0; 
	// 現在通過した境界が「東側」であることを表す定数
	static const int CURRENT_BOUND_EDHE_EAST  = 1; 
	// 現在通過した境界が「南側」であることを表す定数
	static const int CURRENT_BOUND_EDHE_SOUTH = 2; 
	// 現在通過した境界が「西側」であることを表す定数
	static const int CURRENT_BOUND_EDHE_WEST  = 3;

	// 境界の入出を表す変数
	int outOfBoundFlag;
	// 現在通過した境界を表す変数
	int currentBoundEdge;
};


/**
 * トラッキングオブジェクト (マシン) のコースに対する座標情報を
 * MachineObserverを実装したマシン制御用クラスに通知するためのイベントオブジェクトクラス
 * @author   : Junpei TSUJI (DT&T)
 * @version  : 1.0.0-2011.12.25
 */
class ControllDataEvent
{
public:
	// コンストラクタ
	ControllDataEvent(){ controllKey = 0; }

	// キーボード入力の制御キー (イベント発生元に対して通知する用)
	int             controllKey;
	// コース情報のイテレータ
	CourseIterator *iterator;    
	// マシンの現在位置
	CvPoint         currentPoint;     
	// マシンの現在の方向
	cv::Point2d     currentDirection; 
};

