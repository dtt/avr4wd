#pragma once

#include <iostream>
#include <stdio.h>
#include <deque>				//�f�b�N

#include "OpenCV.h"

#include "Course.h"
#include "Calibrator.h"
#include "CourseEditor.h"
#include "Race.h"


//------------------------Invariants----------------------//
#define THRESHOLD 20			//�������l
#define THRESHOLD_MAX_VALUE 255	//�������l�̍ő�l
#define SCALE (1.0/255.0)		//L*a*b�\�F�n�ɕϊ����邽�߂ɕK�v�ȃX�P�[���t�@�N�^


//-------------------------Flags--------------------------//
#define CAPTURE_OFF 0			//�摜�L���v�`�����f
#define CAPTURE_ON 1			//�摜�L���v�`���J�n
#define COLOR_DIFFERENCE 0		//RGB������p���������Z�o
#define LAB_DIFFERENCE 1		//L*a*b�\�F�n��p���������Z�o
#define GRAY_DIFFERENCE 2		//�O���[�X�P�[����p���������Z�o
#define NOISE_KEEP 0			//�m�C�Y���������̃t���O�l
#define NOISE_MORPHOLOGY 1		//�����t�H���W�[���Z��p�����m�C�Y����
#define NOISE_MEDIAN 2			//���f�B�A���t�B���^��p�����m�C�Y����


/**
 * ���[�X�����J�����ɂ���ĒǐՁE�Ǘ����钆���N���X
 * @author   : Sho YAMAUCHI, Junpei TSUJI (DT&T)
 * @version  : 1.0.0-2011.12.25
 */
class RaceTrackingObject : public Race
{
public:
	/**
	 * @bried    : �R���X�g���N�^
	 * @pram[in] : �R�[�X����ǂݍ��ރt�@�C����
	 */
	RaceTrackingObject(std::string filename);
	/**
	 * @bried    : �f�X�g���N�^
	 * @pram[in] : �Ȃ�
	 */
	~RaceTrackingObject(void);


	//----------------Methods-------------------//
	double  getDistance(CvPoint point1, CvPoint point2);
	void    colorDifference(IplImage *currentImage,IplImage *backgroundImage,IplImage *resultImage);
	void    labDifference(IplImage *currentImage,IplImage *backgroundImage,IplImage *resultImage);
	void    grayScaleDifference(IplImage *currentImage,IplImage *backgroundImage,IplImage *resultImage);
	
	void    startRaceTracking();

	CvPoint getCurrentPosition(CvSeq* contour,CvPoint previousPosition);
	int     getCurrentCheckPointIndex(CvPoint currentPoint);
	void    setCurrentDirection();

	// regist observers
	bool attachRaceObserver( RaceObserver* ob );
	bool detachRaceObserver( RaceObserver* ob );
	bool attachMachineObserver( MachineObserver* ob );
	bool detachMachineObserver( MachineObserver* ob );

	// notify
	void updateNotify(UpdateEvent *e);
	void outOfBoundNotify(MachineObserver* ob, OutOfBoundEvent *e);
	void controllDataNotifyAll(ControllDataEvent *e);

	//---------------Fields---------------------//
	CvSize                         imageSize;
	Calibrator                     *calibrator;
	Course                         *course;
	CourseEditor                   *editor;
	RaceObserver                   *raceObserver;
	std::vector<MachineObserver *> *machineObservers;

	CvPoint                        currentPoint;
	CvPoint                        previousPoint;
	cv::Point2d                    currentDirection;
	int                            currentCheckPointIndex;
};

