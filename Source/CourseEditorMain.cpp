/**
 * * �C���N���[�h
 * C:\OpenCV2.3\3rdparty\videoInput\compiledLib\compiledByVS2008\include
 * C:\OpenCV2.3\include
 * 
 * * �ǉ��̃����N�f�B���N�g��
 * C:\OpenCV2.3\3rdparty\videoInput\compiledLib\compiledByVS2008
 * C:\OpenCV2.3\lib\x86\Debug
 *
 * * �ǉ��̈ˑ��t�@�C��
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
 * �R�[�X�쐬���J�n����֐�
 */
void startCourseEdit(Calibrator *calibrator, Course *course, CourseEditor *editor, std::string filename=""){
	int key;

	char windowNameCurrent[]  = "Camera Calibration";
	char windowNameHomo[]     = "Make Your Course";
	char windowNameCourse[]   = "This Is Your Dream Course!!";

	IplImage *frameImage      = NULL;  // �t���[���摜
	IplImage *currentImage    = NULL;  // ���݂̉摜
	IplImage *homoImage       = NULL;  // �z���O���t�B�ϊ���̉摜
	IplImage *courseImage     = NULL;  // �쐬���̃R�[�X�摜

	// �J����
	CameraControl camera;      
	// �J�����摜�T�C�Y
	CvSize cameraImageSize;  

	// �摜�T�C�Y�̐ݒ�
	cameraImageSize = cvSize(640, 480);

	// �J�����̐ݒ�
	if( camera.cameraSetup(0, cameraImageSize.width, cameraImageSize.height) == FALSE ){
		printf( "[Error] Capture device not found!!\n" );
		exit(-1);
	}

	while(true){

		// ���݂̉摜���擾
		frameImage = cvCreateImage(cameraImageSize, IPL_DEPTH_8U,3);
		camera.captureFrameThroughVideoInput(0, frameImage, true);

		// �L�[���͂̊Ǘ�
		key = cvWaitKey(10);
		
		if( calibrator->getHomoMatrix() != NULL ){
			// �R�[�X�쐬���[�h

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
			// �L�����u���[�V�����_�쐬���[�h

			if(currentImage != NULL){
				cvReleaseImage(&currentImage);
			}
			currentImage = cvCloneImage(frameImage);
			
			// �L�����u���[�V�����_��\��
			editor->drawCalibrationPoints(currentImage, calibrator);

			static int flag = 0;
			if(flag == 0){
				// �E�B���h�E��\��
				cvNamedWindow(windowNameCurrent, CV_WINDOW_AUTOSIZE);
				// �R�[���o�b�N�֐��̓o�^
				cvSetMouseCallback(windowNameCurrent, (editor->cameraCalibrationEventHandler), calibrator);  
				flag = 1;
			}

			// �摜��\��
			cvShowImage(windowNameCurrent, currentImage);

		}
		

		if(key == 'q'){
			// �z���O���t�B�s�񂪐�������Ă��� 'q' ���擾���ꂽ�Ƃ��̂ݏI�� 
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
 * ���C���֐�
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
