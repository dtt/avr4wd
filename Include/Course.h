#pragma once

#include <vector>
#include "OpenCV.h"

class CourseIterator;
class Course;

/**
 * マシンが走るコースを定義するクラス
 * @author   : Junpei TSUJI (DT&T)
 * @version  : 1.0.0-2011.12.25
 */
class Course
{
public: 
	/**
	 * @bried    : コンストラクタ
	 * @pram[in] : なし
	 */
	Course(void);
	/**
	 * @bried    : デストラクタ
	 * @pram[in] : なし
	 */
	~Course(void);

	/**
	 * @bried    : コースを一周して列挙するイテレータのポインタを取得するメソッド
	 * @pram[in] : なし
	 * @return   : イテレータのポインタ
	 */
	CourseIterator* getIterator();
	/**
	 * @bried    : コースを一周して列挙するイテレータのポインタを取得するメソッド (初期地点付き)
	 * @pram[in] : イテレータの初期地点
	 * @return   : イテレータのポインタ
	 */
	CourseIterator* getIterator(int indexOfStartPoint);

	/**
	 * @bried    : コースの画像サイズを設定するメソッド
	 * @pram[in] : 画像サイズの横幅, 高さ
	 * @return   : なし
	 */
	void setImageSize(int width, int height);
	/**
	 * @bried    : コースの画像サイズを取得するメソッド
	 * @pram[in] : なし
	 * @return   : 画像サイズの横幅, 高さ
	 */
	CvSize getImageSize();

	/**
	 * @bried    : コース長を返す関数
	 * @pram[in] : なし
	 * @return   : コース長 (整数値)
	 */
	int getCourseLength();

	/**
	 * @bried    : 指定インデックスのコース点を取得するメソッド
	 * @pram[in] : インデックス番号
	 * @return   : コース点の座標
	 */
	CvPoint *getCoursePoint(int index);
	
	/**
	 * @bried    : コース点を追加するメソッド
	 * @pram[in] : 追加するコース点の座標
	 * @return   : なし
	 */
	void addCoursePoint(CvPoint point);

	/**
	 * @bried    : チェックポイント通過と判定する円の半径
	 */
	static const int radiousThreshold = 200;

protected:
	/**
	 * @bried    : コースの画像サイズ
	 */ 
	CvSize imageSize;

	/**
	 * @bried    : コース点の配列 (x0, y0), (x1, y1), ..., (xN, yN)
	 */
	std::vector<CvPoint>  *inputCoursePoints;

};


/**
 * コースを一周分を周回するイテレータクラス
 * 周回する初期地点を指定して生成可能
 * @author   : Junpei TSUJI (DT&T)
 * @version  : 1.0.0-2011.12.25
 */
class CourseIterator 
{
public:
	/**
	 * @bried    : コンストラクタ
	 * @pram[in] : コースのインスタンスと初期地点のインデックス
	 */
	CourseIterator(Course *course, int index);
	/**
	 * @bried    : デストラクタ
	 * @pram[in] : なし
	 */
	~CourseIterator();

	/**
	 * @bried    : 次の要素が存在すればtrue, そうでなければfalseの真偽値を返すメソッド
	 * @pram[in] : なし
	 * @return   : コース上の次の要素の存在に対する真偽値
	 */
	bool hasNext();

	/**
	 * @bried    : 次の要素が存在する場合に限り, コース上の次の要素を取得するメソッド (要 hasNext()メソッド )
	 * @pram[in] : なし
	 * @return   : コース上の次の要素の座標のポインタ
	 */
	CvPoint *getNextPoint();

private:
	/**
	 * @bried    : 現在がスタートから数えて何番目のポイントか表すインデックス変数
	 */
	int index;
	/**
	 * @bried    : スタート地点のインデックス
	 */
	int indexOfStartPoint; 
	/**
	 * @bried    : コースインスタンスへのポインタ
	 */ 
	Course *courseInstance;

};
