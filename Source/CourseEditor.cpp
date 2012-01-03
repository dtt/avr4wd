#include "CourseEditor.h"
	
/**
 * @bried    : �R���X�g���N�^
 * @pram[in] : �L�����u���[�V�����N���X�̃C���X�^���X�ƃR�[�X�N���X�̃C���X�^���X
 */ 
CourseEditor::CourseEditor(Calibrator *calibrator, Course *course){
	//course->setImageSize(640, 480);
	//calibrator->setDestinationImageSize(640, 480);
}

/**
 * @bried    : �R���X�g���N�^ (XML�t�@�C�����L�����u���[�V�����f�[�^�ƃR�[�X�f�[�^���擾����)
 * @pram[in] : �L�����u���[�V�����N���X�̃C���X�^���X�ƃR�[�X�N���X�̃C���X�^���X, �f�[�^���擾����t�@�C����
 */ 
CourseEditor::CourseEditor(Calibrator *calibrator, Course *course, std::string filename)
{	
	this->readFile(calibrator, course, filename); 
}

	
/**
 * @bried    : �f�X�g���N�^
 * @pram[in] : �Ȃ�
 */
CourseEditor::~CourseEditor(void)
{
}



/**
 * @bried    : XML�t�@�C������L�����u���[�V�����f�[�^�ƃR�[�X�f�[�^��ǂݍ���ŃC���X�^���X�𐶐���, �����̃|�C���^�ɑ�����郁�\�b�h
 * @pram[in] : �L�����u���[�V�����N���X�̃C���X�^���X�ƃR�[�X�N���X�̃C���X�^���X, �f�[�^���擾����t�@�C����
 * @return   : �Ȃ�
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

	// �L�����u���[�V�����_���擾
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


	// �R�[�X�_���擾
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
 * @bried    : �L�����u���[�V�����f�[�^�ƃR�[�X�f�[�^��XML�ɏ����o��
 * @pram[in] : �L�����u���[�V�����N���X�̃C���X�^���X�ƃR�[�X�N���X�̃C���X�^���X, �f�[�^��|���o���t�@�C����
 * @return   : �Ȃ�
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
		// ���͉摜�̃L�����u���[�V�����|�C���g�̓o�^
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
		// ���͉摜�̃L�����u���[�V�����|�C���g�̓o�^
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
 * @bried    : �摜�ɃR�[�X�_��`�悷��
 * @pram[in] : �R�[�X��`�悷�錳�摜�̃|�C���^�ƃR�[�X�N���X�̃|�C���^
 * @return   : �Ȃ�
 */  
void CourseEditor::drawCoursePoints(IplImage *image, Course *course){
	CourseIterator *iterator = course->getIterator(0);
	while( iterator->hasNext() ){
		cvCircle (image, *(iterator->getNextPoint()), 3, CV_RGB (0, 0, 255), CV_FILLED);
	}
}

/**
 * @bried    : �摜�ɃR�[�X�̐���`�悷��
 * @pram[in] : �R�[�X��`�悷�錳�摜�̃|�C���^�ƃR�[�X�N���X�̃|�C���^
 * @return   : �Ȃ�
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
 * @bried    : �摜�ɃL�����u���[�V�����_��`�悷��
 * @pram[in] : �`�悷�錳�摜�̃|�C���^�ƃL�����u���[�V�����N���X�̃|�C���^
 * @return   : �Ȃ�
 */  
void CourseEditor::drawCalibrationPoints(IplImage *image, Calibrator *calibrator){
	int n = calibrator->getCalibrationPoints()->size();
	for(int i=0; i<n; i++){
		cvCircle (image, calibrator->getCalibrationPoints()->at(i), 3, CV_RGB (0, 0, 255), CV_FILLED);
	}
}


/**
 * @bried    : �L�����u���[�V�����Ɋւ���}�E�X�N���b�N�C�x���g�n���h��
 * @pram[in] : �}�E�X�N���b�N�C�x���g�̓��͒l
 * @return   : �Ȃ�
 */
void CourseEditor::cameraCalibrationEventHandler( int event, int x, int y, int flags ,void *instance = NULL) // �R�[���o�b�N�֐�
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
 * @bried    : �R�[�X�쐬�ւ���}�E�X�N���b�N�C�x���g�n���h��
 * @pram[in] : �}�E�X�N���b�N�C�x���g�̓��͒l
 * @return   : �Ȃ�
 */
void CourseEditor::createCourseEventHandler( int event, int x, int y, int flags ,void *course = NULL) // �R�[���o�b�N�֐�
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
