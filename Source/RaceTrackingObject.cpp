#include "RaceTrackingObject.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind/bind.hpp>



void BOOST_SLEEP(int n){
	boost::thread::sleep(boost::get_system_time() + boost::posix_time::milliseconds((n)));
}

RaceTrackingObject::RaceTrackingObject(std::string filename)
{
	// �L�����u���[�V�������C�R�[�X�����t�@�C������擾
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
�w�i�����@�����s���郁�\�b�h
*/
void RaceTrackingObject::startRaceTracking(){

	int key;			// �L�[�{�[�h���͊i�[�p		

	/*
	�E�B���h�E�̖��O
	*/
	char windowNameCurrent[]    = "Current";
	char windowNameResult[]     = "Result";
	char windowNameBackground[] = "Background";
	char windowNameHomo[]       = "Homography";
	
	/*
	�摜�i�[�p�ϐ�
	*/
	IplImage *frameImage = NULL;
	IplImage *currentImage = NULL;
	IplImage *backgroundImage = NULL;
	IplImage *resultImage = NULL;
	IplImage *homoImage       = NULL;  // �z���O���t�B�ϊ���̉摜
	IplImage *currentCloneImage = NULL;

	/*
	���[�h�ݒ�
	*/
	int captureOn = CAPTURE_ON;
	int differenceMode = GRAY_DIFFERENCE;
	//int noiseMode = NOISE_KEEP;
	//int noiseMode = NOISE_MEDIAN;
	int noiseMode = NOISE_MORPHOLOGY;

	/*
	�\�����b�Z�[�W
	*/
	char *differenceMethod[3]={
		"RGB�̐������Ƃɕ]��",
		"CIE L*a*b* �ŋ�����]��",
		"�O���[�X�P�[���ŕ]��"
	};

	char *noiseMethod[3]={
		"�Ȃ�",
		"�I�[�v�j���O",
		"���f�B�A���t�B���^"
	};


	CameraControl camCon;					    //VideoInput�𗘗p�����J�����R���g���[���N���X

	/*
	�E�B���h�E����
	*/
	cvNamedWindow(windowNameCurrent,CV_WINDOW_AUTOSIZE);	
	cvNamedWindow(windowNameResult, CV_WINDOW_AUTOSIZE);	

	/*
	�J�����摜�擾
	*/
	frameImage = cvCreateImage(cvSize(
						camCon.defaultInputCameraSizeWidth,  
						camCon.defaultInputCameraSizeHeight), 
					IPL_DEPTH_8U,
					3);	  //�摜���������iRGB�R�`���l���j
	camCon.cameraSetup(0,frameImage->width,frameImage->height);					//�J�����̃Z�b�g�A�b�v
	camCon.captureFrameThroughVideoInput(0,frameImage,true);					//�摜���J��������擾

	/*
	�e��摜�̏�����
	*/
	backgroundImage = cvCloneImage(frameImage);									//�w�i�摜�Ƃ��Ď擾�����摜���R�s�[

	currentImage = cvCloneImage(frameImage);									//���݂̉摜�Ƃ��Ď擾�����摜���R�s�[

	currentCloneImage = cvCloneImage(currentImage);								//�\���p�摜�Ƃ��ăR�s�[

	imageSize = cvSize(frameImage->width,frameImage->height);					//���ʂ̉摜�T�C�Y��CvSize�Ƃ��ĕۑ�

	resultImage = cvCreateImage(imageSize,IPL_DEPTH_8U,1);						//�w�i�����̌��ʂ�\������摜���쐬

	CvSize size = calibrator->getDestinationImageSize();
	homoImage = cvCreateImage( size, IPL_DEPTH_8U, 3 );
		
	int previousPointX = 0;														//�g���b�L���O�̑O��ʒu��X���W��ێ�����ϐ�
	int previousPointY = 0;														//�g���b�L���O�̑O��ʒu��Y���W��ێ�����ϐ�	

	/*
	�w�i�������s�����C�����[�v
	*/
	while(true){
	
		camCon.captureFrameThroughVideoInput(0,frameImage,true);				//�摜���J��������擾

		/*
		�O��̃��[�v�����ŉ摜���擾����Ă���Ε\���p�摜��j��
						�@�摜���擾����Ă��Ȃ���Ε\���p�摜���擾�����摜����R�s�[
		*/
		if(captureOn == CAPTURE_ON){	//�J�����摜�擾���ł���
			if(currentImage != NULL){			//�摜���擾����Ă���ꍇ
				cvReleaseImage(&currentImage);		//�O��摜��j��
				cvReleaseImage(&currentCloneImage);
			}
			currentImage = cvCloneImage(frameImage);	//�擾�����摜�����݂̉摜�Ƃ��ăR�s�[
			currentCloneImage = cvCloneImage(currentImage);

			editor->drawCalibrationPoints(currentCloneImage, calibrator);
		}

		/*
		���[�h���Ƃɔw�i�摜�ƌ��݂̉摜�̍����Z�o����
		*/
		if(differenceMode == COLOR_DIFFERENCE){							//RGB�e������p���������Z�o��@
			colorDifference(currentImage,backgroundImage,resultImage);
		}else if(differenceMode == LAB_DIFFERENCE){						//L*a*b�\�F�n��p���������Z�o��@
			labDifference(currentImage,backgroundImage,resultImage);
		}else if(differenceMode == GRAY_DIFFERENCE){
			grayScaleDifference(currentImage,backgroundImage,resultImage);	//�O���[�X�P�[����p���������Z�o��@
		}

		/*
		���[�h���ƂɃm�C�Y��������
		*/
		if(noiseMode == NOISE_MORPHOLOGY){		//�����t�H���W�[���Z�ɂ��m�C�Y��������
			cvErode(resultImage,resultImage);	//�����t�H���W�[���Z�@���k
			cvDilate(resultImage,resultImage);	//�����t�H���W�[���Z�@�c��
		}else if(noiseMode == NOISE_MEDIAN){	//���f�B�A���t�B���^��p�����m�C�Y��������
			cvSmooth(resultImage,resultImage,CV_MEDIAN);	//���f�B�A���t�B���^��p����������
		}

		CvMemStorage* storage = cvCreateMemStorage(0);			//�֊s�擾�̂��߂̃������̈�m��
		CvSeq* findContour = NULL;								//�֊s���i�[�ϐ�
		IplImage* resultCloneImage = cvCloneImage(resultImage);	//�w�i�������ʉ摜�̃R�s�[

		/*
		�֊s���o����
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
		���C���\���p�F���
		*/
		CvScalar red = CV_RGB(255,0,0);

		/*
		���o�����֊s�ɑ΂��鏈��
		*/

		if(findContourNum > 0){//�֊s�����o���ꂽ��

			CvPoint point = this->getCurrentPosition(findContour,cvPoint(previousPointX,previousPointY));	//���݂̈ʒu�𐄒肷��

			/*
			���肵���ʒu���J�����̕\���̈���Ȃ�΂��̈ʒu�ƈړ��̋O�Ղ�`��
			*/
			if(point.x > 0 && point.x < frameImage->width){
				if(point.y > 0 && point.y < frameImage->height){
					cvLine(currentCloneImage,point,cvPoint(previousPointX,previousPointY),red,3,CV_AA,0);//���݈ʒu�ƂP�X�e�b�v�O�̈ʒu�ԂɐԂ���������
					/*
					���̃��[�v�����̂��߂ɍ��W���X�V
					*/
					previousPointX = point.x;
					previousPointY = point.y;
				}
			}
			cvClearSeq(findContour);//�֊s���̔j��
		}

		cvReleaseImage(&resultCloneImage);//�w�i�������ʂ̃R�s�[�摜��j��
	
		cvReleaseMemStorage(&storage);	//�֊s���̒��o�ɗp�����������̈���J��


		/*
		�摜���E�C���h�E�ɕ\��
		*/
		cvShowImage(windowNameCurrent,currentCloneImage);
		cvShowImage(windowNameResult, resultImage);

		/*
		�L�[���͏���
		*/
		key = cvWaitKey(10);	//�L�[�̎擾

		if(key == 'q'){			//�I��
			break;
		}else if(key == 'b'){	//�w�i�摜�X�V
			if(backgroundImage != NULL){	//�w�i�摜�����݂���Δj��
				cvReleaseImage(&backgroundImage);
			}
			backgroundImage = cvCloneImage(frameImage);	//�w�i�摜���擾�摜�ɃA�b�v�f�[�g
			printf("�w�i�摜�X�V\n");					//���b�Z�[�W
		}else if(key == ' '){							//�摜�擾ONOFF
			captureOn = 1 - captureOn;					//�l�𔽓]�i0��1�j
			if(captureOn == CAPTURE_ON){				//�摜�擾�J�n
				printf("�摜�擾:ON\n");				//���b�Z�[�W
			}else if(captureOn == CAPTURE_OFF){			//�摜�擾���f
				printf("�摜�擾:OFF\n");				//���b�Z�[�W
			}
		}else if(key == 'm'){							//�����v�Z���@��ύX
			differenceMode = differenceMode + 1;		//�e���[�h�����[�e�[�V����
			if(differenceMode > GRAY_DIFFERENCE){
				differenceMode = COLOR_DIFFERENCE;
			}
			printf("���̕]�����@:%s\n",differenceMethod[differenceMode]);	//���b�Z�[�W
		}else if(key == 'n'){							//�m�C�Y�������@��ύX
			noiseMode = noiseMode + 1;					//�e���[�h�����[�e�[�V����
			if(noiseMode > NOISE_MEDIAN){
				noiseMode = NOISE_KEEP;
			}
			printf("�m�C�Y�������@:%s\n",noiseMethod[noiseMode]);		//���b�Z�[�W
		}
		// �ˉe�ϊ��s�񂪂ł��Ă���Ƃ�
		if( calibrator->getHomoMatrix() != NULL ){
			
			// �ˉe�ϊ��s��Ɋ�Â��摜��ϊ�
			calibrator->homographyImageTranslation(currentImage, homoImage);
			
			// �ˉe�ϊ��s��Ɋ�Â��}�V���̈ʒu��ϊ�
			CvPoint srcPoint = cvPoint(previousPointX, previousPointY);
			CvPoint dstPoint = calibrator->homographyPointTranslation( srcPoint );
			this->currentPoint = dstPoint;

			// �}�V���̕������ʒu�̕ω��Ɋ�Â��v�Z
			this->setCurrentDirection();

			// �^�[�Q�b�g�̃R�[�X�`�F�b�N�|�C���g�̃C���f�b�N�X���X�V���擾
			int checkPointIndex = this->getCurrentCheckPointIndex( this->currentPoint );
						
			// �ʒu���̃C�x���g��ʒm
			UpdateEvent *ue = new UpdateEvent();             // �C�x���g�̐���
			ue->controllKey = key;                           // ���ʂ̃R���g���[���L�[�̒ʒm
			std::vector<int> *indexes = new std::vector<int>();
			indexes->push_back(this->currentCheckPointIndex);
			ue->currentCheckPointIndexArray = indexes;       // ���݂̖ړI�R�[�X�`�F�b�N�|�C���g�̔z���ʒm
			std::vector<CvPoint> *points = new std::vector<CvPoint>();
			points->push_back(this->currentPoint);
			ue->currentPositionArray = points;               // �ʒu����̒ʒm
			std::vector<cv::Point2d> *directions = new std::vector<cv::Point2d>();
			directions->push_back(this->currentDirection);
			ue->currentDirectionArray = directions;          // ������̒ʒm
			ue->image  = homoImage;                          // �摜�̒ʒm
			ue->course = this->course;                       // �R�[�X���̒ʒm
			this->updateNotify(ue);                          // �ʒm����
			delete points;                                   // �������̊J��
			delete directions;                               // �������̊J��
			delete ue;                                       // �������̊J��

			// �}�V���ɃC�x���g��ʒm
			ControllDataEvent *ce = new ControllDataEvent(); // �C�x���g�̐���
			ce->controllKey       = key;                     // ���ʂ̃R���g���[���L�[�̒ʒm
			ce->iterator          = ( this->course->getIterator( checkPointIndex ) );
			ce->currentPoint      =  this->currentPoint;
			ce->currentDirection  =  this->currentDirection;
			this->controllDataNotifyAll(ce);                 // �ʒm����
			delete ce->iterator;                             // �������̊J��
			delete ce;                                       // �������̊J��

			// ���E�Ɋւ���C�x���g��ʒm
			static int outOfBoundFlag = OutOfBoundEvent::OUT_OF_BOUND_FLAG_UNKNOWN;
			if( 0 <= dstPoint.x && dstPoint.x <= course->getImageSize().width && 0 <= dstPoint.y && dstPoint.y <= course->getImageSize().height ){
				if( outOfBoundFlag != OutOfBoundEvent::OUT_OF_BOUND_FLAG_IN ){
					// ���E�ɓ������C�x���g��ʒm
					OutOfBoundEvent *oe = new OutOfBoundEvent();
					oe->outOfBoundFlag = OutOfBoundEvent::OUT_OF_BOUND_FLAG_IN;
					this->outOfBoundNotify(this->machineObservers->at(0), oe);
					delete oe;
					outOfBoundFlag = OutOfBoundEvent::OUT_OF_BOUND_FLAG_IN;
				}
			}
			else {
				if( outOfBoundFlag != OutOfBoundEvent::OUT_OF_BOUND_FLAG_OUT ){
					// ���E���o���C�x���g��ʒm
					OutOfBoundEvent *oe = new OutOfBoundEvent();
					oe->outOfBoundFlag = OutOfBoundEvent::OUT_OF_BOUND_FLAG_OUT;
					this->outOfBoundNotify(this->machineObservers->at(0), oe);
					delete oe;
					outOfBoundFlag = OutOfBoundEvent::OUT_OF_BOUND_FLAG_OUT;
				}
			}

		}

		// ���Ԋu�ŃX���[�v
		BOOST_SLEEP(20);
	}


	/*
	�ێ����Ă���摜��j��
	*/
	cvReleaseImage(&backgroundImage);
	cvReleaseImage(&currentImage);
	cvReleaseImage(&currentCloneImage);
	cvReleaseImage(&resultImage);
	cvReleaseImage(&homoImage);

	/*
	�E�C���h�E��j��
	*/
	cvDestroyWindow(windowNameCurrent);
	cvDestroyWindow(windowNameResult);
}


/**
 * ���݂̕�����ݒ肷��֐�
 */ 
void RaceTrackingObject::setCurrentDirection(){
	this->currentDirection.x = currentPoint.x - previousPoint.x;
	this->currentDirection.y = currentPoint.y - previousPoint.y;
	this->previousPoint = this->currentPoint;
}


/**
 * 臒l���a���Ƀ}�V�������B����ΖڕW���X�V���C�ڕW��index�ԍ���Ԃ��֐�
 */
int RaceTrackingObject::getCurrentCheckPointIndex(CvPoint currentPoint){

	int n = this->course->getCourseLength();
	
	// ���݂̖ڕW�_��ݒ�
	CvPoint targetPoint = *(this->course->getCoursePoint( (this->currentCheckPointIndex + 1) % n ));

	// �ڕW�ɓ��B����ΖڕW�_���X�V
	if( RaceTrackingObject::getDistance(currentPoint, targetPoint) <= Course::radiousThreshold ){
		this->currentCheckPointIndex = (this->currentCheckPointIndex + 1) % n;
	}
	return this->currentCheckPointIndex;
}



/*
RGB������p���������Z�o���\�b�h
�����͌��摜��currentImage,�w�i�摜��backgroundImage,���ʂ��i�[������IplImage�ϐ���resultImage�Ƃ��邱��
*/
void RaceTrackingObject::colorDifference(IplImage *currentImage,IplImage *backgroundImage,IplImage *resultImage){

	//�摜�̕ϐ��̐���
	IplImage *differenceImage = cvCreateImage(imageSize,IPL_DEPTH_8U,3);
	IplImage *differenceRImage = cvCreateImage(imageSize,IPL_DEPTH_8U,1);//R�p
	IplImage *differenceGImage = cvCreateImage(imageSize,IPL_DEPTH_8U,1);//G�p
	IplImage *differenceBImage = cvCreateImage(imageSize,IPL_DEPTH_8U,1);//B�p

	//���摜�Ɣw�i�摜�Ƃ̍��𐬕����ƂɌv�Z���AdiffereceImage�Ɍ��ʂ��i�[
	cvAbsDiff(currentImage,backgroundImage,differenceImage);
	//�������l����
	cvThreshold(differenceImage,differenceImage,THRESHOLD,THRESHOLD_MAX_VALUE,CV_THRESH_BINARY);
	//RGB�e�������Ƃɉ摜�𕪊�
	cvSplit(differenceImage,differenceBImage,differenceGImage,differenceRImage,NULL);
	//OR���Z�ɂ�鍇���i�i�P�jR��G��OR�@�i�Q�j�i�P�j�̌��ʂ�B��OR�j�A���ʂ�resultImage�Ɋi�[
	cvOr(differenceRImage,differenceGImage,resultImage);
	cvOr(differenceBImage,resultImage,resultImage);
	//�摜��j��
	cvReleaseImage(&differenceImage);
	cvReleaseImage(&differenceRImage);
	cvReleaseImage(&differenceGImage);
	cvReleaseImage(&differenceBImage);
}

/*
L*a*b�\�F�n��p���������Z�o���\�b�h
�����͌��摜��currentImage,�w�i�摜��backgroundImage,���ʂ��i�[������IplImage�ϐ���resultImage�Ƃ��邱��
*/
void RaceTrackingObject::labDifference(IplImage *currentImage,IplImage *backgroundImage,IplImage *resultImage){

	/*
	�摜�𐶐�
	*/
	IplImage *currentLabImage = cvCreateImage(imageSize,IPL_DEPTH_32F,3);	//���摜�p
	IplImage *backgroundLabImage = cvCreateImage(imageSize,IPL_DEPTH_32F,3);//�w�i�摜�p
	IplImage *differenceLabImage = cvCreateImage(imageSize,IPL_DEPTH_32F,3);//�����摜�p

	IplImage *differenceLImage = cvCreateImage(imageSize,IPL_DEPTH_32F,1);//L*�p
	IplImage *differenceAImage = cvCreateImage(imageSize,IPL_DEPTH_32F,1);//a*�p
	IplImage *differenceBImage = cvCreateImage(imageSize,IPL_DEPTH_32F,1);//b*�p

	IplImage *sqrDifferenceImage = cvCreateImage(imageSize,IPL_DEPTH_32F,1);//�����Z�o�p

	/*
	���݂̉摜��CIE L*a*b*�ɕϊ�����
	*/
	cvConvertScale(currentImage,currentLabImage,SCALE);
	cvConvertScale(backgroundImage,backgroundLabImage,SCALE);
	cvCvtColor(currentLabImage,currentLabImage,CV_BGR2Lab);
	cvCvtColor(backgroundLabImage,backgroundLabImage,CV_BGR2Lab);

	/*
	�����̓����Z�o����
	*/
	cvSub(currentLabImage,backgroundLabImage,differenceLabImage);
	cvPow(differenceLabImage,differenceLabImage,2);

	//�������Ƃɉ摜�𕪊�
	cvSplit(differenceLabImage,differenceLImage,differenceAImage,differenceBImage,NULL);

	cvCopy(differenceLImage,sqrDifferenceImage);
	cvAdd(differenceAImage,sqrDifferenceImage,sqrDifferenceImage);
	cvAdd(differenceBImage,sqrDifferenceImage,sqrDifferenceImage);

	//�������l�������s��
	cvThreshold(sqrDifferenceImage,resultImage,THRESHOLD*THRESHOLD,THRESHOLD_MAX_VALUE,CV_THRESH_BINARY);

	/*
	�摜�̔j��
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
�O���[�X�P�[����p���������Z�o���\�b�h
�����͌��摜��currentImage,�w�i�摜��backgroundImage,���ʂ��i�[������IplImage�ϐ���resultImage�Ƃ��邱��
*/
void RaceTrackingObject::grayScaleDifference(IplImage *currentImage,IplImage *backgroundImage,IplImage *resultImage){
	//�摜�̐���
	IplImage *differenceImage = cvCreateImage(imageSize,IPL_DEPTH_8U,3);
	//���摜�Ɣw�i�摜�̍��̐�Βl���e�������ƂɌv�Z�A���ʂ�differenceImage�Ɋi�[
	cvAbsDiff(currentImage,backgroundImage,differenceImage);
	//BGR����O���[�X�P�[���ɕϊ�
	cvCvtColor(differenceImage,resultImage,CV_BGR2GRAY);
	//�������l�����i�Q�l���j
	cvThreshold(resultImage,resultImage,THRESHOLD,THRESHOLD_MAX_VALUE,CV_THRESH_BINARY);
	//�摜�̔j��
	cvReleaseImage(&differenceImage);
}

/*
CvPoint�Q�_�Ԃ̋������v�Z���郁�\�b�h
*/
double RaceTrackingObject::getDistance(CvPoint point1, CvPoint point2){

	double x = point1.x - point2.x;
	double y = point1.y - point2.y;

	return sqrt(x*x + y*y);
}

/*
�d�ݕt�����ς��Ƃ��@��p���Č��݈ʒu�𐄒肷�郁�\�b�h
*/
CvPoint RaceTrackingObject::getCurrentPosition(CvSeq* contour, CvPoint previousPosition){

	double totalValue = 0;			

	/*
	�֊s���Ƃ̏����i�[����f�b�N
	*/
	std::deque<CvPoint> positions;
	std::deque<double> values;	

	/*
	���o���ꂽ�֊s���Ƃɏd�݂��v�Z����
	*/
	for(CvSeq* currentContour = contour;currentContour != NULL;currentContour = currentContour->h_next){//���X�g�\���̑���
		
		double x = 0,y = 0;

		for(int i = 0;i < currentContour->total;i++){		//���݂̗֊s���\������S�Ă̓_�ɑ΂��ď���
			CvPoint *point = CV_GET_SEQ_ELEM(CvPoint,currentContour,i);		//i�Ԗڂ̓_�̍��W�𓾂�
			/*
			���̗֊s�̏d�S�𓾂邽��x,y���ꂼ���S�ĉ��Z
			*/
			x += point->x;
			y += point->y;
		}
		/*
		�_�̌��Ŋ��邱�Ƃŏd�S�𓾂�
		*/
		x /= currentContour->total;
		y /= currentContour->total;
		positions.push_back(cvPoint(x,y));	//�d�S��ۑ�
		
		/*
		�d�݂��v�Z����
		*/
		double dist = this->getDistance(cvPoint(x,y),previousPosition);//�P�X�e�b�v�O�̐���ʒu�Əd�S�̋������v�Z
		double area = fabs(cvContourArea(currentContour, CV_WHOLE_SEQ));//���݂̗֊s�̖ʐς𓾂�
		values.push_back((area * area + 1)/(dist * dist + 1));	//�d�݂��v�Z���ĕۑ�

	}
	/*
	�d�݂̑��a���v�Z
	*/
	for(int i = 0;i < positions.size();i++){
		totalValue += values[i];
	}

	double pointX = 0,pointY = 0;

	/*
	�֊s���ƂɎZ�o�����d�S�ʒu�A�d�݂����Ɍ��݂̈ʒu�𐄒�
	*/
	for(int i = 0;i < positions.size();i++){
		pointX += (values[i] / totalValue) * positions[i].x;
		pointY += (values[i] / totalValue) * positions[i].y;
	}
	
	/*
	���݂̈ʒu��CvPoint�Ƃ��ă��^�[��
	*/
	CvPoint point = cvPoint(pointX,pointY);

	return point;

}


/**
 * @brief     : observer��ǉ�����֐�
 * @param[in] : observer�N���X
 * @return    : ��������
 */
bool RaceTrackingObject::attachRaceObserver(RaceObserver *observer){
	this->raceObserver = observer;
	return true;
}
/**
 * @brief    : observer���폜����֐�
 * @pram[in] : observer�N���X
 * @return   : ��������
 */
bool RaceTrackingObject::detachRaceObserver(RaceObserver *observer){
	this->raceObserver = NULL;
	return true;
}

/**
 * @brief     : MachineObserver��ǉ�����֐�
 * @param[in] : MachineObserver�N���X
 * @return    : ��������
 */
bool RaceTrackingObject::attachMachineObserver(MachineObserver* ob ){
	this->machineObservers->push_back(ob);
	
	return true;
}

/**
 * @brief    : MachineObserver���폜����֐�
 * @pram[in] : MachineObserver�N���X
 * @return   : ��������
 */
bool RaceTrackingObject::detachMachineObserver( MachineObserver* ob ){
	
	return true;
}


/**
 * @bried    : observer��updateEventHandler���Ăяo���֐�
 * @pram[in] : UpdateEvent�^�̃C�x���g�I�u�W�F�N�g
 * @return   : �Ȃ�
 */
void RaceTrackingObject::updateNotify(UpdateEvent *e){
	if( this->raceObserver != NULL ){
		this->raceObserver->updateEventHandler(e);
	}
}

/**
 * @bried    : observer��outOfBoundEventHandler���Ăяo���֐�
 * @pram[in] : OutOfBoundEvent�^�̃C�x���g�I�u�W�F�N�g
 * @return   : �Ȃ�
 */
void RaceTrackingObject::outOfBoundNotify(MachineObserver *observer, OutOfBoundEvent *e){
	if( observer != NULL ){
		observer->outOfBoundEventHandler(e);
	}
}

/**
 * @bried    : observer��controllDataEventHandler���Ăяo���֐�
 * @pram[in] : ControllDataEvent�^�̃C�x���g�I�u�W�F�N�g
 * @return   : �Ȃ�
 */
void RaceTrackingObject::controllDataNotifyAll(ControllDataEvent *e){
	if( this->machineObservers != NULL ){
		for(int i=0; i<this->machineObservers->size(); i++){
			this->machineObservers->at(i)->controllDataEventHandler(e);
		}
	}
}

