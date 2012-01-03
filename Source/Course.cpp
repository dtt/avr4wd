#include "Course.h"

/**
 * @bried    : コンストラクタ
 * @pram[in] : なし
 */
Course::Course(){
	this->setImageSize(320, 240);
	this->inputCoursePoints = new std::vector<CvPoint>();
}
/**
 * @bried    : デストラクタ
 * @pram[in] : なし
 */
Course::~Course(void){
	delete this->inputCoursePoints;
}

/**
 * @bried    : コースを一周して列挙するイテレータのポインタを取得するメソッド
 * @pram[in] : なし
 * @return   : イテレータのポインタ
 */
CourseIterator* Course::getIterator(){
	return new CourseIterator(this, 0);
}
/**
 * @bried    : コースを一周して列挙するイテレータのポインタを取得するメソッド (初期地点付き)
 * @pram[in] : イテレータの初期地点
 * @return   : イテレータのポインタ
 */
CourseIterator* Course::getIterator(int indexOfStartPoint){
	return new CourseIterator(this, indexOfStartPoint);
}

/**
 * @bried    : コースの画像サイズを設定するメソッド
 * @pram[in] : 画像サイズの横幅, 高さ
 * @return   : なし
 */
void Course::setImageSize(int width, int height){
	this->imageSize = cvSize(width, height);
}
/**
 * @bried    : コースの画像サイズを取得するメソッド
 * @pram[in] : なし
 * @return   : 画像サイズの横幅, 高さ
 */
CvSize Course::getImageSize(){
	return this->imageSize;
}

/**
 * @bried    : コース長を返す関数
 * @pram[in] : なし
 * @return   : コース長 (整数値)
 */ 
int Course::getCourseLength(){
	return this->inputCoursePoints->size();
}


/**
 * @bried    : 指定インデックスのコース点を取得するメソッド
 * @pram[in] : インデックス番号
 * @return   : コース点の座標
 */
CvPoint *Course::getCoursePoint(int index){
	if( index < this->getCourseLength() ){
		return &(this->inputCoursePoints->at(index));
	}
	else{
		return NULL;
	}
}

/**
 * @bried    : コース点を追加するメソッド
 * @pram[in] : 追加するコース点の座標
 * @return   : なし
 */
void Course::addCoursePoint(CvPoint point){
	this->inputCoursePoints->push_back(point);
}


/**
 * @bried    : コンストラクタ
 * @pram[in] : コースのインスタンスと初期地点のインデックス
 */
CourseIterator::CourseIterator(Course *course, int index){
	this->indexOfStartPoint = index;
	this->courseInstance = course;
	this->index = 0;
}
/**
 * @bried    : デストラクタ
 * @pram[in] : なし
 */
CourseIterator::~CourseIterator(){
}

/**
 * @bried    : 次の要素が存在すればtrue, そうでなければfalseの真偽値を返すメソッド
 * @pram[in] : なし
 * @return   : コース上の次の要素の存在に対する真偽値
 */
bool CourseIterator::hasNext(){
	if( this->index < this->courseInstance->getCourseLength() ){
		return true;
	}
	else{
		return false;
	}
}

/**
 * @bried    : 次の要素が存在する場合に限り, コース上の次の要素を取得するメソッド (要 hasNext()メソッド )
 * @pram[in] : なし
 * @return   : コース上の次の要素の座標のポインタ
 */
CvPoint *CourseIterator::getNextPoint(){
	CvPoint *point = this->courseInstance->getCoursePoint( (indexOfStartPoint+index) % this->courseInstance->getCourseLength() );
	index++;
	return point;
}
