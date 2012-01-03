#pragma once

#include "Event.h"
#include "RaceObserver.h"
#include "MachineObserver.h"

/**
 * レース情報を管理するクラスのインタフェース
 * @author   : Junpei TSUJI (DT&T)
 * @version  : 1.0.0-2011.12.25
 */
class Race
{
public:
	/**
	 * @brief     : RaceObserverを追加する関数
	 * @param[in] : RaceObserverクラス
	 * @return    : 成功判定
	 */
	virtual bool attachRaceObserver( RaceObserver* ob )=0;

	/**
	 * @brief    : RaceObserverを削除する関数
	 * @pram[in] : RaceObserverクラス
	 * @return   : 成功判定
	 */
	virtual bool detachRaceObserver( RaceObserver* ob )=0;

	/**
	 * @brief     : MachineObserverを追加する関数
	 * @param[in] : MachineObserverクラス
	 * @return    : 成功判定
	 */
	virtual bool attachMachineObserver( MachineObserver* ob )=0;

	/**
	 * @brief    : MachineObserverを削除する関数
	 * @pram[in] : MachineObserverクラス
	 * @return   : 成功判定
	 */
	virtual bool detachMachineObserver( MachineObserver* ob )=0;

	/**
	 * @bried    : RaceObserverのupdateEventHandlerを呼び出す関数
	 * @pram[in] : UpdateEvent型のイベントオブジェクト
	 * @return   : なし
	 */
	virtual void updateNotify(UpdateEvent *e)=0;

	/**
	 * @bried    : MachineObserverのoutOfBoundEventHandlerを呼び出す関数
	 * @pram[in] : OutOfBoundEvent型のイベントオブジェクト
	 * @return   : なし
	 */
	virtual void outOfBoundNotify(MachineObserver* ob, OutOfBoundEvent *e)=0;

	/**
	 * @bried    : MachineObserverのcontrollDataEventHandlerを呼び出す関数
	 * @pram[in] : ControllDataEvent型のイベントオブジェクト
	 * @return   : なし
	 */
	virtual void controllDataNotifyAll(ControllDataEvent *e)=0;


	virtual void startRaceTracking()=0;

};

