#pragma once

#include "RaceObserver.h"
#include "Event.h"


/**
 * レース情報を
 * @author   : Junpei TSUJI (DT&T)
 * @version  : 1.0.0-2011.12.25
 */
class RaceViewer : public RaceObserver
{
public:
	/**
	 * @bried    : コンストラクタ
	 * @pram[in] : なし
	 */
	RaceViewer();
	/**
	 * @bried    : デストラクタ
	 * @pram[in] : なし
	 */
	~RaceViewer();

	/**
	 * @bried    : 位置の更新が観測されるときに起動するイベントハンドラ
	 * @pram[in] : UpdateEvent型のイベントオブジェクト
	 * @return   : なし
	 */
	void updateEventHandler(UpdateEvent *e);

	/**
	 * @bried    : IplImageにコースを書き込むメソッド
	 * @pram[in] : IplImage, Course 型のオブジェクト
	 * @return   : なし
	 */
	void drawCourse(IplImage *image, Course *course);
	
};
