#pragma once
#include "Event.h"

/**
 * マシン単体の状態を監視するためのオブザーバークラス
 * @author   : Junpei TSUJI (DT&T)
 * @version  : 1.0.0-2011.12.25
 */
class MachineObserver 
{
public:
	/**
	 * @bried    : トラッキングオブジェクトが境界を超えた時に起動するイベントハンドラ
	 * @pram[in] : OutOfBoundEvent型のイベントオブジェクト
	 * @return   : なし
	 */
	virtual void outOfBoundEventHandler(OutOfBoundEvent *e)=0;

	/**
	 * @bried    : コースとの座標を受け取って起動するイベントハンドラ
	 * @pram[in] : ControllDataEvent型のイベントオブジェクト
	 * @return   : なし
	 */
	virtual void controllDataEventHandler(ControllDataEvent *e)=0;	

};


