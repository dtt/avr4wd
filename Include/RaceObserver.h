#pragma once
#include "Event.h"


/**
 * レースを監視するオブザーバークラスのインタフェース
 * @author   : Junpei TSUJI (DT&T)
 * @version  : 1.0.0-2011.12.25
 */
class RaceObserver 
{
public:
	/**
	 * @bried    : 位置の更新が観測されるときに起動するイベントハンドラ
	 * @pram[in] : UpdateEvent型のイベントオブジェクト
	 * @return   : なし
	 */
	virtual void updateEventHandler(UpdateEvent *e)=0;

};

