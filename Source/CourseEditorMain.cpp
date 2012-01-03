/**
 * * インクルード
 * C:\OpenCV2.3\3rdparty\videoInput\compiledLib\compiledByVS2008\include
 * C:\OpenCV2.3\include
 * 
 * * 追加のリンクディレクトリ
 * C:\OpenCV2.3\3rdparty\videoInput\compiledLib\compiledByVS2008
 * C:\OpenCV2.3\lib\x86\Debug
 *
 * * 追加の依存ファイル
 * opencv_core230d.lib
 * opencv_imgproc230d.lib
 * opencv_highgui230d.lib
 * opencv_calib3d230d.lib
 * videoInput.lib
 * 
 */


#include "CourseEditor.h"

//#define __EDIT_MODE__
#define __WRITE_MODE__


#ifdef __EDIT_MODE__
const std::string _LOG_FILENAME = "log.xml";

/**
 * コース作成を開始する関数
 */
void startCourseEdit(Calibrator *calibrator, Course *course, CourseEditor *editor, std::string filename=""){
	int key;

	char windowNameCurrent[]  = "Camera Calibration";
	char windowNameHomo[]     = "Make Your Course";
	char windowNameCourse[]   = "This Is Your Dream Course!!";

	IplImage *frameImage      = NULL;  // フレーム画像
	IplImage *currentImage    = NULL;  // 現在の画像
	IplImage *homoImage       = NULL;  // ホモグラフィ変換後の画像
	IplImage *courseImage     = NULL;  // 作成中のコース画像

	// カメラ
	CameraControl camera;      
	// カメラ画像サイズ
	CvSize cameraImageSize;  

	// 画像サイズの設定
	cameraImageSize = cvSize(640, 480);

	// カメラの設定
	if( camera.cameraSetup(0, cameraImageSize.width, cameraImageSize.height) == FALSE ){
		printf( "[Error] Capture device not found!!\n" );
		exit(-1);
	}

	while(true){

		// 現在の画像を取得
		frameImage = cvCreateImage(cameraImageSize, IPL_DEPTH_8U,3);
		camera.captureFrameThroughVideoInput(0, frameImage, true);

		// キー入力の管理
		key = cvWaitKey(10);
		
		if( calibrator->getHomoMatrix() != NULL ){
			// コース作成モード

			static int flag = 0;
			if(flag == 0){
				cvDestroyWindow(windowNameCurrent);
				cvNamedWindow(windowNameHomo,CV_WINDOW_AUTOSIZE);
				cvSetMouseCallback(windowNameHomo, (editor->createCourseEventHandler), course);  
				flag = 1;
			}

			if(homoImage != NULL){ cvReleaseImage(&homoImage); }
			homoImage = cvCreateImage( calibrator->getDestinationImageSize(), IPL_DEPTH_8U, 3 );
			calibrator->homographyImageTranslation( frameImage, homoImage );

			if(courseImage != NULL){ cvReleaseImage(&courseImage); }
			courseImage = cvCreateImage( course->getImageSize(), IPL_DEPTH_8U, 3 );

			editor->drawCoursePoints(homoImage, course);
			editor->drawCourseLines(homoImage, course);
			editor->drawCoursePoints(courseImage, course);
			editor->drawCourseLines(courseImage, course);

			cvShowImage(windowNameHomo,    homoImage);
			cvShowImage(windowNameCourse,  courseImage);

		}
		else{
			// キャリブレーション点作成モード

			if(currentImage != NULL){
				cvReleaseImage(&currentImage);
			}
			currentImage = cvCloneImage(frameImage);
			
			// キャリブレーション点を表示
			editor->drawCalibrationPoints(currentImage, calibrator);

			static int flag = 0;
			if(flag == 0){
				// ウィンドウを表示
				cvNamedWindow(windowNameCurrent, CV_WINDOW_AUTOSIZE);
				// コールバック関数の登録
				cvSetMouseCallback(windowNameCurrent, (editor->cameraCalibrationEventHandler), calibrator);  
				flag = 1;
			}

			// 画像を表示
			cvShowImage(windowNameCurrent, currentImage);

		}
		

		if(key == 'q'){
			// ホモグラフィ行列が生成されていて 'q' が取得されたときのみ終了 
			break;
		}
	}

	if( filename.compare("") != 0 ){
		editor->writeFile(calibrator, course, filename);
		std::cout << "# file has written." << std::endl;
	}
	else{
		std::cout << "# file has NOT written." << std::endl;

	}

	cvReleaseImage(&frameImage);
	cvReleaseImage(&currentImage);
	cvReleaseImage(&homoImage);
	cvReleaseImage(&courseImage);

	cvDestroyWindow(windowNameHomo);
	cvDestroyWindow(windowNameCourse);
}



/**
 * メイン関数
 */
int main(int argc, char *argv[]){
	Calibrator *calibrator = new Calibrator();
	Course *course = new Course();

#ifdef __EDIT_MODE__
	course->setImageSize(480, 640);
	calibrator->setDestinationImageSize(480, 640);
	CourseEditor *editor = new CourseEditor(calibrator, course);
#else
	CourseEditor *editor = new CourseEditor(calibrator, course, _LOG_FILENAME);
#endif


#ifdef __WRITE_MODE__
	startCourseEdit(calibrator, course, editor, _LOG_FILENAME);
#else
	startCourseEdit(calibrator, course, editor);
#endif

	delete calibrator;	delete course;	delete editor;
	return 0;
}

#endif
