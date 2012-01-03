#include "RaceTrackingObject.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind/bind.hpp>



void BOOST_SLEEP(int n){
	boost::thread::sleep(boost::get_system_time() + boost::posix_time::milliseconds((n)));
}

RaceTrackingObject::RaceTrackingObject(std::string filename)
{
	// キャリブレーション情報，コース情報をファイルから取得
	calibrator = new Calibrator();
	course = new Course();
	editor = new CourseEditor(calibrator, course, filename);

	this->raceObserver = NULL;
	this->machineObservers = new std::vector<MachineObserver *>();
	this->currentCheckPointIndex = 0;
}


RaceTrackingObject::~RaceTrackingObject(void)
{
	delete calibrator;	
	delete course;	
	delete editor;

	delete machineObservers;
}


/*
背景差分法を実行するメソッド
*/
void RaceTrackingObject::startRaceTracking(){

	int key;			// キーボード入力格納用		

	/*
	ウィンドウの名前
	*/
	char windowNameCurrent[]    = "Current";
	char windowNameResult[]     = "Result";
	char windowNameBackground[] = "Background";
	char windowNameHomo[]       = "Homography";
	
	/*
	画像格納用変数
	*/
	IplImage *frameImage = NULL;
	IplImage *currentImage = NULL;
	IplImage *backgroundImage = NULL;
	IplImage *resultImage = NULL;
	IplImage *homoImage       = NULL;  // ホモグラフィ変換後の画像
	IplImage *currentCloneImage = NULL;

	/*
	モード設定
	*/
	int captureOn = CAPTURE_ON;
	int differenceMode = GRAY_DIFFERENCE;
	//int noiseMode = NOISE_KEEP;
	//int noiseMode = NOISE_MEDIAN;
	int noiseMode = NOISE_MORPHOLOGY;

	/*
	表示メッセージ
	*/
	char *differenceMethod[3]={
		"RGBの成分ごとに評価",
		"CIE L*a*b* で距離を評価",
		"グレースケールで評価"
	};

	char *noiseMethod[3]={
		"なし",
		"オープニング",
		"メディアンフィルタ"
	};


	CameraControl camCon;					    //VideoInputを利用したカメラコントロールクラス

	/*
	ウィンドウ生成
	*/
	cvNamedWindow(windowNameCurrent,CV_WINDOW_AUTOSIZE);	
	cvNamedWindow(windowNameResult, CV_WINDOW_AUTOSIZE);	

	/*
	カメラ画像取得
	*/
	frameImage = cvCreateImage(cvSize(
						camCon.defaultInputCameraSizeWidth,  
						camCon.defaultInputCameraSizeHeight), 
					IPL_DEPTH_8U,
					3);	  //画像を初期化（RGB３チャネル）
	camCon.cameraSetup(0,frameImage->width,frameImage->height);					//カメラのセットアップ
	camCon.captureFrameThroughVideoInput(0,frameImage,true);					//画像をカメラから取得

	/*
	各種画像の初期化
	*/
	backgroundImage = cvCloneImage(frameImage);									//背景画像として取得した画像をコピー

	currentImage = cvCloneImage(frameImage);									//現在の画像として取得した画像をコピー

	currentCloneImage = cvCloneImage(currentImage);								//表示用画像としてコピー

	imageSize = cvSize(frameImage->width,frameImage->height);					//共通の画像サイズをCvSizeとして保存

	resultImage = cvCreateImage(imageSize,IPL_DEPTH_8U,1);						//背景差分の結果を表示する画像を作成

	CvSize size = calibrator->getDestinationImageSize();
	homoImage = cvCreateImage( size, IPL_DEPTH_8U, 3 );
		
	int previousPointX = 0;														//トラッキングの前回位置のX座標を保持する変数
	int previousPointY = 0;														//トラッキングの前回位置のY座標を保持する変数	

	/*
	背景差分を行うメインループ
	*/
	while(true){
	
		camCon.captureFrameThroughVideoInput(0,frameImage,true);				//画像をカメラから取得

		/*
		前回のループ処理で画像が取得されていれば表示用画像を破棄
						　画像が取得されていなければ表示用画像を取得した画像からコピー
		*/
		if(captureOn == CAPTURE_ON){	//カメラ画像取得中である
			if(currentImage != NULL){			//画像が取得されている場合
				cvReleaseImage(&currentImage);		//前回画像を破棄
				cvReleaseImage(&currentCloneImage);
			}
			currentImage = cvCloneImage(frameImage);	//取得した画像を現在の画像としてコピー
			currentCloneImage = cvCloneImage(currentImage);

			editor->drawCalibrationPoints(currentCloneImage, calibrator);
		}

		/*
		モードごとに背景画像と現在の画像の差分算出処理
		*/
		if(differenceMode == COLOR_DIFFERENCE){							//RGB各成分を用いた差分算出手法
			colorDifference(currentImage,backgroundImage,resultImage);
		}else if(differenceMode == LAB_DIFFERENCE){						//L*a*b表色系を用いた差分算出手法
			labDifference(currentImage,backgroundImage,resultImage);
		}else if(differenceMode == GRAY_DIFFERENCE){
			grayScaleDifference(currentImage,backgroundImage,resultImage);	//グレースケールを用いた差分算出手法
		}

		/*
		モードごとにノイズ除去処理
		*/
		if(noiseMode == NOISE_MORPHOLOGY){		//モルフォロジー演算によるノイズ除去処理
			cvErode(resultImage,resultImage);	//モルフォロジー演算　収縮
			cvDilate(resultImage,resultImage);	//モルフォロジー演算　膨張
		}else if(noiseMode == NOISE_MEDIAN){	//メディアンフィルタを用いたノイズ除去処理
			cvSmooth(resultImage,resultImage,CV_MEDIAN);	//メディアンフィルタを用いた平滑化
		}

		CvMemStorage* storage = cvCreateMemStorage(0);			//輪郭取得のためのメモリ領域確保
		CvSeq* findContour = NULL;								//輪郭情報格納変数
		IplImage* resultCloneImage = cvCloneImage(resultImage);	//背景差分結果画像のコピー

		/*
		輪郭抽出処理
		*/
		int findContourNum = cvFindContours(
			resultCloneImage,
			storage,
			&findContour,
			sizeof(CvContour),
			CV_RETR_CCOMP,
			CV_CHAIN_APPROX_NONE,
			cvPoint(0,0)
			);

		/*
		ライン表示用色情報
		*/
		CvScalar red = CV_RGB(255,0,0);

		/*
		抽出した輪郭に対する処理
		*/

		if(findContourNum > 0){//輪郭が抽出された時

			CvPoint point = this->getCurrentPosition(findContour,cvPoint(previousPointX,previousPointY));	//現在の位置を推定する

			/*
			推定した位置がカメラの表示領域内ならばその位置と移動の軌跡を描画
			*/
			if(point.x > 0 && point.x < frameImage->width){
				if(point.y > 0 && point.y < frameImage->height){
					cvLine(currentCloneImage,point,cvPoint(previousPointX,previousPointY),red,3,CV_AA,0);//現在位置と１ステップ前の位置間に赤い線を引く
					/*
					次のループ処理のために座標を更新
					*/
					previousPointX = point.x;
					previousPointY = point.y;
				}
			}
			cvClearSeq(findContour);//輪郭情報の破棄
		}

		cvReleaseImage(&resultCloneImage);//背景差分結果のコピー画像を破棄
	
		cvReleaseMemStorage(&storage);	//輪郭情報の抽出に用いたメモリ領域を開放


		/*
		画像をウインドウに表示
		*/
		cvShowImage(windowNameCurrent,currentCloneImage);
		cvShowImage(windowNameResult, resultImage);

		/*
		キー入力処理
		*/
		key = cvWaitKey(10);	//キーの取得

		if(key == 'q'){			//終了
			break;
		}else if(key == 'b'){	//背景画像更新
			if(backgroundImage != NULL){	//背景画像が存在すれば破棄
				cvReleaseImage(&backgroundImage);
			}
			backgroundImage = cvCloneImage(frameImage);	//背景画像を取得画像にアップデート
			printf("背景画像更新\n");					//メッセージ
		}else if(key == ' '){							//画像取得ONOFF
			captureOn = 1 - captureOn;					//値を反転（0か1）
			if(captureOn == CAPTURE_ON){				//画像取得開始
				printf("画像取得:ON\n");				//メッセージ
			}else if(captureOn == CAPTURE_OFF){			//画像取得中断
				printf("画像取得:OFF\n");				//メッセージ
			}
		}else if(key == 'm'){							//差分計算方法を変更
			differenceMode = differenceMode + 1;		//各モードをローテーション
			if(differenceMode > GRAY_DIFFERENCE){
				differenceMode = COLOR_DIFFERENCE;
			}
			printf("差の評価方法:%s\n",differenceMethod[differenceMode]);	//メッセージ
		}else if(key == 'n'){							//ノイズ除去方法を変更
			noiseMode = noiseMode + 1;					//各モードをローテーション
			if(noiseMode > NOISE_MEDIAN){
				noiseMode = NOISE_KEEP;
			}
			printf("ノイズ除去方法:%s\n",noiseMethod[noiseMode]);		//メッセージ
		}
		// 射影変換行列ができているとき
		if( calibrator->getHomoMatrix() != NULL ){
			
			// 射影変換行列に基づき画像を変換
			calibrator->homographyImageTranslation(currentImage, homoImage);
			
			// 射影変換行列に基づきマシンの位置を変換
			CvPoint srcPoint = cvPoint(previousPointX, previousPointY);
			CvPoint dstPoint = calibrator->homographyPointTranslation( srcPoint );
			this->currentPoint = dstPoint;

			// マシンの方向を位置の変化に基づき計算
			this->setCurrentDirection();

			// ターゲットのコースチェックポイントのインデックスを更新し取得
			int checkPointIndex = this->getCurrentCheckPointIndex( this->currentPoint );
						
			// 位置情報のイベントを通知
			UpdateEvent *ue = new UpdateEvent();             // イベントの生成
			ue->controllKey = key;                           // 共通のコントロールキーの通知
			std::vector<int> *indexes = new std::vector<int>();
			indexes->push_back(this->currentCheckPointIndex);
			ue->currentCheckPointIndexArray = indexes;       // 現在の目的コースチェックポイントの配列を通知
			std::vector<CvPoint> *points = new std::vector<CvPoint>();
			points->push_back(this->currentPoint);
			ue->currentPositionArray = points;               // 位置情報列の通知
			std::vector<cv::Point2d> *directions = new std::vector<cv::Point2d>();
			directions->push_back(this->currentDirection);
			ue->currentDirectionArray = directions;          // 方向列の通知
			ue->image  = homoImage;                          // 画像の通知
			ue->course = this->course;                       // コース情報の通知
			this->updateNotify(ue);                          // 通知処理
			delete points;                                   // メモリの開放
			delete directions;                               // メモリの開放
			delete ue;                                       // メモリの開放

			// マシンにイベントを通知
			ControllDataEvent *ce = new ControllDataEvent(); // イベントの生成
			ce->controllKey       = key;                     // 共通のコントロールキーの通知
			ce->iterator          = ( this->course->getIterator( checkPointIndex ) );
			ce->currentPoint      =  this->currentPoint;
			ce->currentDirection  =  this->currentDirection;
			this->controllDataNotifyAll(ce);                 // 通知処理
			delete ce->iterator;                             // メモリの開放
			delete ce;                                       // メモリの開放

			// 境界に関するイベントを通知
			static int outOfBoundFlag = OutOfBoundEvent::OUT_OF_BOUND_FLAG_UNKNOWN;
			if( 0 <= dstPoint.x && dstPoint.x <= course->getImageSize().width && 0 <= dstPoint.y && dstPoint.y <= course->getImageSize().height ){
				if( outOfBoundFlag != OutOfBoundEvent::OUT_OF_BOUND_FLAG_IN ){
					// 境界に入ったイベントを通知
					OutOfBoundEvent *oe = new OutOfBoundEvent();
					oe->outOfBoundFlag = OutOfBoundEvent::OUT_OF_BOUND_FLAG_IN;
					this->outOfBoundNotify(this->machineObservers->at(0), oe);
					delete oe;
					outOfBoundFlag = OutOfBoundEvent::OUT_OF_BOUND_FLAG_IN;
				}
			}
			else {
				if( outOfBoundFlag != OutOfBoundEvent::OUT_OF_BOUND_FLAG_OUT ){
					// 境界を出たイベントを通知
					OutOfBoundEvent *oe = new OutOfBoundEvent();
					oe->outOfBoundFlag = OutOfBoundEvent::OUT_OF_BOUND_FLAG_OUT;
					this->outOfBoundNotify(this->machineObservers->at(0), oe);
					delete oe;
					outOfBoundFlag = OutOfBoundEvent::OUT_OF_BOUND_FLAG_OUT;
				}
			}

		}

		// 一定間隔でスリープ
		BOOST_SLEEP(20);
	}


	/*
	保持している画像を破棄
	*/
	cvReleaseImage(&backgroundImage);
	cvReleaseImage(&currentImage);
	cvReleaseImage(&currentCloneImage);
	cvReleaseImage(&resultImage);
	cvReleaseImage(&homoImage);

	/*
	ウインドウを破棄
	*/
	cvDestroyWindow(windowNameCurrent);
	cvDestroyWindow(windowNameResult);
}


/**
 * 現在の方向を設定する関数
 */ 
void RaceTrackingObject::setCurrentDirection(){
	this->currentDirection.x = currentPoint.x - previousPoint.x;
	this->currentDirection.y = currentPoint.y - previousPoint.y;
	this->previousPoint = this->currentPoint;
}


/**
 * 閾値半径内にマシンが到達すれば目標を更新し，目標のindex番号を返す関数
 */
int RaceTrackingObject::getCurrentCheckPointIndex(CvPoint currentPoint){

	int n = this->course->getCourseLength();
	
	// 現在の目標点を設定
	CvPoint targetPoint = *(this->course->getCoursePoint( (this->currentCheckPointIndex + 1) % n ));

	// 目標に到達すれば目標点を更新
	if( RaceTrackingObject::getDistance(currentPoint, targetPoint) <= Course::radiousThreshold ){
		this->currentCheckPointIndex = (this->currentCheckPointIndex + 1) % n;
	}
	return this->currentCheckPointIndex;
}



/*
RGB成分を用いた差分算出メソッド
引数は元画像をcurrentImage,背景画像をbackgroundImage,結果を格納したいIplImage変数をresultImageとすること
*/
void RaceTrackingObject::colorDifference(IplImage *currentImage,IplImage *backgroundImage,IplImage *resultImage){

	//画像の変数の生成
	IplImage *differenceImage = cvCreateImage(imageSize,IPL_DEPTH_8U,3);
	IplImage *differenceRImage = cvCreateImage(imageSize,IPL_DEPTH_8U,1);//R用
	IplImage *differenceGImage = cvCreateImage(imageSize,IPL_DEPTH_8U,1);//G用
	IplImage *differenceBImage = cvCreateImage(imageSize,IPL_DEPTH_8U,1);//B用

	//元画像と背景画像との差を成分ごとに計算し、differeceImageに結果を格納
	cvAbsDiff(currentImage,backgroundImage,differenceImage);
	//しきい値処理
	cvThreshold(differenceImage,differenceImage,THRESHOLD,THRESHOLD_MAX_VALUE,CV_THRESH_BINARY);
	//RGB各成分ごとに画像を分割
	cvSplit(differenceImage,differenceBImage,differenceGImage,differenceRImage,NULL);
	//OR演算による合成（（１）RとGでOR　（２）（１）の結果とBでOR）、結果をresultImageに格納
	cvOr(differenceRImage,differenceGImage,resultImage);
	cvOr(differenceBImage,resultImage,resultImage);
	//画像を破棄
	cvReleaseImage(&differenceImage);
	cvReleaseImage(&differenceRImage);
	cvReleaseImage(&differenceGImage);
	cvReleaseImage(&differenceBImage);
}

/*
L*a*b表色系を用いた差分算出メソッド
引数は元画像をcurrentImage,背景画像をbackgroundImage,結果を格納したいIplImage変数をresultImageとすること
*/
void RaceTrackingObject::labDifference(IplImage *currentImage,IplImage *backgroundImage,IplImage *resultImage){

	/*
	画像を生成
	*/
	IplImage *currentLabImage = cvCreateImage(imageSize,IPL_DEPTH_32F,3);	//元画像用
	IplImage *backgroundLabImage = cvCreateImage(imageSize,IPL_DEPTH_32F,3);//背景画像用
	IplImage *differenceLabImage = cvCreateImage(imageSize,IPL_DEPTH_32F,3);//差分画像用

	IplImage *differenceLImage = cvCreateImage(imageSize,IPL_DEPTH_32F,1);//L*用
	IplImage *differenceAImage = cvCreateImage(imageSize,IPL_DEPTH_32F,1);//a*用
	IplImage *differenceBImage = cvCreateImage(imageSize,IPL_DEPTH_32F,1);//b*用

	IplImage *sqrDifferenceImage = cvCreateImage(imageSize,IPL_DEPTH_32F,1);//距離算出用

	/*
	現在の画像をCIE L*a*b*に変換する
	*/
	cvConvertScale(currentImage,currentLabImage,SCALE);
	cvConvertScale(backgroundImage,backgroundLabImage,SCALE);
	cvCvtColor(currentLabImage,currentLabImage,CV_BGR2Lab);
	cvCvtColor(backgroundLabImage,backgroundLabImage,CV_BGR2Lab);

	/*
	距離の二乗を算出する
	*/
	cvSub(currentLabImage,backgroundLabImage,differenceLabImage);
	cvPow(differenceLabImage,differenceLabImage,2);

	//成分ごとに画像を分割
	cvSplit(differenceLabImage,differenceLImage,differenceAImage,differenceBImage,NULL);

	cvCopy(differenceLImage,sqrDifferenceImage);
	cvAdd(differenceAImage,sqrDifferenceImage,sqrDifferenceImage);
	cvAdd(differenceBImage,sqrDifferenceImage,sqrDifferenceImage);

	//しきい値処理を行う
	cvThreshold(sqrDifferenceImage,resultImage,THRESHOLD*THRESHOLD,THRESHOLD_MAX_VALUE,CV_THRESH_BINARY);

	/*
	画像の破棄
	*/
	cvReleaseImage(&currentLabImage);
	cvReleaseImage(&backgroundLabImage);
	cvReleaseImage(&differenceLabImage);
	cvReleaseImage(&differenceLImage);
	cvReleaseImage(&differenceAImage);
	cvReleaseImage(&differenceBImage);
	cvReleaseImage(&sqrDifferenceImage);
}

/*
グレースケールを用いた差分算出メソッド
引数は元画像をcurrentImage,背景画像をbackgroundImage,結果を格納したいIplImage変数をresultImageとすること
*/
void RaceTrackingObject::grayScaleDifference(IplImage *currentImage,IplImage *backgroundImage,IplImage *resultImage){
	//画像の生成
	IplImage *differenceImage = cvCreateImage(imageSize,IPL_DEPTH_8U,3);
	//元画像と背景画像の差の絶対値を各成分ごとに計算、結果をdifferenceImageに格納
	cvAbsDiff(currentImage,backgroundImage,differenceImage);
	//BGRからグレースケールに変換
	cvCvtColor(differenceImage,resultImage,CV_BGR2GRAY);
	//しきい値処理（２値化）
	cvThreshold(resultImage,resultImage,THRESHOLD,THRESHOLD_MAX_VALUE,CV_THRESH_BINARY);
	//画像の破棄
	cvReleaseImage(&differenceImage);
}

/*
CvPoint２点間の距離を計算するメソッド
*/
double RaceTrackingObject::getDistance(CvPoint point1, CvPoint point2){

	double x = point1.x - point2.x;
	double y = point1.y - point2.y;

	return sqrt(x*x + y*y);
}

/*
重み付け平均をとる手法を用いて現在位置を推定するメソッド
*/
CvPoint RaceTrackingObject::getCurrentPosition(CvSeq* contour, CvPoint previousPosition){

	double totalValue = 0;			

	/*
	輪郭ごとの情報を格納するデック
	*/
	std::deque<CvPoint> positions;
	std::deque<double> values;	

	/*
	抽出された輪郭ごとに重みを計算する
	*/
	for(CvSeq* currentContour = contour;currentContour != NULL;currentContour = currentContour->h_next){//リスト構造の走査
		
		double x = 0,y = 0;

		for(int i = 0;i < currentContour->total;i++){		//現在の輪郭を構成する全ての点に対して処理
			CvPoint *point = CV_GET_SEQ_ELEM(CvPoint,currentContour,i);		//i番目の点の座標を得る
			/*
			この輪郭の重心を得るためx,yそれぞれを全て加算
			*/
			x += point->x;
			y += point->y;
		}
		/*
		点の個数で割ることで重心を得る
		*/
		x /= currentContour->total;
		y /= currentContour->total;
		positions.push_back(cvPoint(x,y));	//重心を保存
		
		/*
		重みを計算する
		*/
		double dist = this->getDistance(cvPoint(x,y),previousPosition);//１ステップ前の推定位置と重心の距離を計算
		double area = fabs(cvContourArea(currentContour, CV_WHOLE_SEQ));//現在の輪郭の面積を得る
		values.push_back((area * area + 1)/(dist * dist + 1));	//重みを計算して保存

	}
	/*
	重みの総和を計算
	*/
	for(int i = 0;i < positions.size();i++){
		totalValue += values[i];
	}

	double pointX = 0,pointY = 0;

	/*
	輪郭ごとに算出した重心位置、重みを元に現在の位置を推定
	*/
	for(int i = 0;i < positions.size();i++){
		pointX += (values[i] / totalValue) * positions[i].x;
		pointY += (values[i] / totalValue) * positions[i].y;
	}
	
	/*
	現在の位置をCvPointとしてリターン
	*/
	CvPoint point = cvPoint(pointX,pointY);

	return point;

}


/**
 * @brief     : observerを追加する関数
 * @param[in] : observerクラス
 * @return    : 成功判定
 */
bool RaceTrackingObject::attachRaceObserver(RaceObserver *observer){
	this->raceObserver = observer;
	return true;
}
/**
 * @brief    : observerを削除する関数
 * @pram[in] : observerクラス
 * @return   : 成功判定
 */
bool RaceTrackingObject::detachRaceObserver(RaceObserver *observer){
	this->raceObserver = NULL;
	return true;
}

/**
 * @brief     : MachineObserverを追加する関数
 * @param[in] : MachineObserverクラス
 * @return    : 成功判定
 */
bool RaceTrackingObject::attachMachineObserver(MachineObserver* ob ){
	this->machineObservers->push_back(ob);
	
	return true;
}

/**
 * @brief    : MachineObserverを削除する関数
 * @pram[in] : MachineObserverクラス
 * @return   : 成功判定
 */
bool RaceTrackingObject::detachMachineObserver( MachineObserver* ob ){
	
	return true;
}


/**
 * @bried    : observerのupdateEventHandlerを呼び出す関数
 * @pram[in] : UpdateEvent型のイベントオブジェクト
 * @return   : なし
 */
void RaceTrackingObject::updateNotify(UpdateEvent *e){
	if( this->raceObserver != NULL ){
		this->raceObserver->updateEventHandler(e);
	}
}

/**
 * @bried    : observerのoutOfBoundEventHandlerを呼び出す関数
 * @pram[in] : OutOfBoundEvent型のイベントオブジェクト
 * @return   : なし
 */
void RaceTrackingObject::outOfBoundNotify(MachineObserver *observer, OutOfBoundEvent *e){
	if( observer != NULL ){
		observer->outOfBoundEventHandler(e);
	}
}

/**
 * @bried    : observerのcontrollDataEventHandlerを呼び出す関数
 * @pram[in] : ControllDataEvent型のイベントオブジェクト
 * @return   : なし
 */
void RaceTrackingObject::controllDataNotifyAll(ControllDataEvent *e){
	if( this->machineObservers != NULL ){
		for(int i=0; i<this->machineObservers->size(); i++){
			this->machineObservers->at(i)->controllDataEventHandler(e);
		}
	}
}

