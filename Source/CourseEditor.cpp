#include "CourseEditor.h"
	
/**
 * @bried    : コンストラクタ
 * @pram[in] : キャリブレーションクラスのインスタンスとコースクラスのインスタンス
 */ 
CourseEditor::CourseEditor(Calibrator *calibrator, Course *course){
	//course->setImageSize(640, 480);
	//calibrator->setDestinationImageSize(640, 480);
}

/**
 * @bried    : コンストラクタ (XMLファイルよりキャリブレーションデータとコースデータを取得する)
 * @pram[in] : キャリブレーションクラスのインスタンスとコースクラスのインスタンス, データを取得するファイル名
 */ 
CourseEditor::CourseEditor(Calibrator *calibrator, Course *course, std::string filename)
{	
	this->readFile(calibrator, course, filename); 
}

	
/**
 * @bried    : デストラクタ
 * @pram[in] : なし
 */
CourseEditor::~CourseEditor(void)
{
}



/**
 * @bried    : XMLファイルからキャリブレーションデータとコースデータを読み込んでインスタンスを生成し, 引数のポインタに代入するメソッド
 * @pram[in] : キャリブレーションクラスのインスタンスとコースクラスのインスタンス, データを取得するファイル名
 * @return   : なし
 */
void CourseEditor::readFile(Calibrator *calibrator, Course *course, std::string filename){
	CvFileStorage *cvfs;
	CvFileNode *node;

	// memory space for loading data
	int width;
	int height;
	CvMat *homoMatrix = (CvMat*)cvAlloc(sizeof(CvMat*));

	// open file storage
	cvfs = cvOpenFileStorage(filename.c_str(), NULL, CV_STORAGE_READ);

	// read data from file storage
	node = cvGetFileNodeByName(cvfs, NULL, "");   // Get Top Node

	width  = cvReadIntByName(cvfs, node, "width", 0);

	height = cvReadIntByName(cvfs, node, "height", 0);
	calibrator->setDestinationImageSize(width, height);
	course->setImageSize(width, height);

	// キャリブレーション点を取得
	std::cout << "# calibration points:" << std::endl;
	CvMat *calibrationMatrix = (CvMat*)cvAlloc(sizeof(CvMat*));
	calibrationMatrix =  (CvMat*)cvReadByName(cvfs, node, "calibrationPoints", NULL);

	std::vector<int> *calibration_points = new std::vector<int>(); 
	for(int i=0; i<calibrationMatrix->rows; i++){
		for(int j=0; j<calibrationMatrix->cols; j++){
			std::cout << cvmGet(calibrationMatrix, i, j) << ", ";  
			calibration_points->push_back( (int)cvmGet(calibrationMatrix, i, j) );
		}
		std::cout << std::endl;
	}
	int num = calibration_points->size() / 2;
	for(int i=0; i<num; i++){
		int poi1, poi2;
		poi1 = (int)calibration_points->at(2*i);
		poi2 = (int)calibration_points->at(2*i+1);
		calibrator->addCalibrationPoint(cvPoint(poi1, poi2));
		std::cout << "p: " << poi1 << ", " << poi2 << std::endl;
	}
	delete calibration_points;

	calibrator->createHomoMatrix();


	// コース点を取得
	std::cout << "# course points:" << std::endl;
	CvMat *courseMatrix = (CvMat*)cvAlloc(sizeof(CvMat*));
	courseMatrix =  (CvMat*)cvReadByName(cvfs, node, "coursePoints", NULL);

	std::vector<int> *course_points = new std::vector<int>(); 

	for(int i=0; i<courseMatrix->rows; i++){
		for(int j=0; j<courseMatrix->cols; j++){
			std::cout << cvmGet(courseMatrix, i, j) << ", ";  
			course_points->push_back( (int)cvmGet(courseMatrix, i, j) );
		}
		std::cout << std::endl;
	}
	num = course_points->size() / 2;
	for(int i=0; i<num; i++){
		int poi1, poi2;
		poi1 = (int)course_points->at(2*i);
		poi2 = (int)course_points->at(2*i+1);
		course->addCoursePoint(cvPoint(poi1, poi2));
		std::cout << "p: " << poi1 << ", " << poi2 << std::endl;
	}
	delete course_points;




	// close file storage
	cvReleaseFileStorage(&cvfs);
}

/**
 * @bried    : キャリブレーションデータとコースデータをXMLに書き出し
 * @pram[in] : キャリブレーションクラスのインスタンスとコースクラスのインスタンス, データを掃き出すファイル名
 * @return   : なし
 */	
void CourseEditor::writeFile(Calibrator *calibrator, Course *course, std::string filename){
	CvFileStorage *cvfs;


	// open file storage
	cvfs = cvOpenFileStorage(filename.c_str(), NULL,CV_STORAGE_WRITE);

	// write data to file
	cvWriteInt(cvfs, "width",  calibrator->getDestinationImageSize().width);
	cvWriteInt(cvfs, "height", calibrator->getDestinationImageSize().height); 

	if( calibrator->getHomoMatrix() != NULL ){
		/*
		cvWrite(cvfs, "homoMatrix", calibrator->getHomoMatrix(), cvAttrList(NULL, NULL));
		*/

		CvMat    src_point;
		std::vector<CvPoint> *points = calibrator->getCalibrationPoints();

		float    a[] = {	
			(float)points->at(0).x, (float)points->at(0).y,
			(float)points->at(1).x, (float)points->at(1).y,
			(float)points->at(2).x, (float)points->at(2).y,
			(float)points->at(3).x, (float)points->at(3).y,

		};
		// 入力画像のキャリブレーションポイントの登録
		src_point = cvMat( calibrator->CALIBRATION_NUM, 2, CV_32FC1, a );

		cvWrite(cvfs, "calibrationPoints", &src_point, cvAttrList(NULL, NULL));


		CvMat    course_point;
		int course_point_size = course->getCourseLength();

		float *course_array = NULL;
		course_array  = new float[2*course_point_size];
		for(int i=0; i<course_point_size; i++){
			course_array[2*i]   = (float)(*(course->getCoursePoint(i))).x;
			course_array[2*i+1] = (float)(*(course->getCoursePoint(i))).y;
		}
		// 入力画像のキャリブレーションポイントの登録
		course_point = cvMat( course_point_size, 2, CV_32FC1, course_array );
		//delete course_array;
		cvWrite(cvfs, "coursePoints", &course_point, cvAttrList(NULL, NULL));

		if( course_array != NULL ){
			delete course_array;
		}

	}


	// close file storage
	cvReleaseFileStorage(&cvfs);
}



/**
 * @bried    : 画像にコース点を描画する
 * @pram[in] : コースを描画する元画像のポインタとコースクラスのポインタ
 * @return   : なし
 */  
void CourseEditor::drawCoursePoints(IplImage *image, Course *course){
	CourseIterator *iterator = course->getIterator(0);
	while( iterator->hasNext() ){
		cvCircle (image, *(iterator->getNextPoint()), 3, CV_RGB (0, 0, 255), CV_FILLED);
	}
}

/**
 * @bried    : 画像にコースの線を描画する
 * @pram[in] : コースを描画する元画像のポインタとコースクラスのポインタ
 * @return   : なし
 */ 
void CourseEditor::drawCourseLines(IplImage *image, Course *course){
	CourseIterator *iterator = course->getIterator(0);
	CvPoint *prevPoint;
	
	if( iterator->hasNext() ) prevPoint = iterator->getNextPoint();
	while( iterator->hasNext() ){
		CvPoint *point = (iterator->getNextPoint());
		cvLine (image, *prevPoint, *point, CV_RGB (0, 0, 255));
		prevPoint = point;
	}

	int length = course->getCourseLength();
	if( length > 2 ){
		cvLine (image, *(course->getCoursePoint(length-1)), *(course->getCoursePoint(0)), CV_RGB (0, 0, 255));
	}
}



/**
 * @bried    : 画像にキャリブレーション点を描画する
 * @pram[in] : 描画する元画像のポインタとキャリブレーションクラスのポインタ
 * @return   : なし
 */  
void CourseEditor::drawCalibrationPoints(IplImage *image, Calibrator *calibrator){
	int n = calibrator->getCalibrationPoints()->size();
	for(int i=0; i<n; i++){
		cvCircle (image, calibrator->getCalibrationPoints()->at(i), 3, CV_RGB (0, 0, 255), CV_FILLED);
	}
}


/**
 * @bried    : キャリブレーションに関するマウスクリックイベントハンドラ
 * @pram[in] : マウスクリックイベントの入力値
 * @return   : なし
 */
void CourseEditor::cameraCalibrationEventHandler( int event, int x, int y, int flags ,void *instance = NULL) // コールバック関数
{
	switch(event)
	{
	case CV_EVENT_LBUTTONDOWN:
		{

			if( instance != NULL ){

				Calibrator *pthis = reinterpret_cast<Calibrator*>(instance);

				pthis->addCalibrationPoint(cvPoint(x,y));

				int num = pthis->getCalibrationPoints()->size();
				if(  num >= pthis->CALIBRATION_NUM ){
					pthis->createHomoMatrix();
				}
				
			}
			break;
		}

	default:
		break;
	}

}


/**
 * @bried    : コース作成関するマウスクリックイベントハンドラ
 * @pram[in] : マウスクリックイベントの入力値
 * @return   : なし
 */
void CourseEditor::createCourseEventHandler( int event, int x, int y, int flags ,void *course = NULL) // コールバック関数
{
	switch(event)
	{
	case CV_EVENT_LBUTTONDOWN:
		{
			if( course != NULL ){
				Course *pcourse = reinterpret_cast<Course*>(course);
				pcourse->addCoursePoint(cvPoint(x, y));
			}
			break;
		}
	default:
		break;
	}

}
