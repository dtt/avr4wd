#pragma once

#include "MachineObserver.h"
#include "Event.h"
#include "SerialPort.h"


/**
 * マシンをシリアル通信で操作するためのクラス
 * @author   : Junpei TSUJI (DT&T)
 * @version  : 1.0.0-2011.12.25
 */
class MachineController : public MachineObserver, public serial::SerialObserver
{
public:
	/**
	 * @bried    : コンストラクタ
	 * @pram[in] : COMポートの文字列とボーレートの数値
	 */
	MachineController(std::string comPort, int baudRate);

	/**
	 * @bried    : デストラクタ
	 */ 
	~MachineController();

	/**
	 * @bried    : シリアル通信を受信するイベントハンドラ
	 * @pram[in] : 受信する文字列
	 * @return   : なし
	 */ 
	void notify( const std::string& str );

	/**
	 * @bried    : トラッキングオブジェクトが境界を超えた時に起動するイベントハンドラ
	 * @pram[in] : OutOfBoundEvent型のイベントオブジェクト
	 * @return   : なし
	 */
	void outOfBoundEventHandler(OutOfBoundEvent *e);

	/**
	 * @bried    : コースとの相対座標を受け取って起動するイベントハンドラ
	 * @pram[in] : ControllDataEvent型のイベントオブジェクト
	 * @return   : なし
	 */
	void controllDataEventHandler(ControllDataEvent *e);	

protected:
	/**
	 * @bried    : シリアル通信のインスタンス
	 */
	serial::SerialPort *serial;  
};
