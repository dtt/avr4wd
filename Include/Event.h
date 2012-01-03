#pragma once
#include "Course.h"

/**
 * �g���b�L���O�I�u�W�F�N�g (�}�V��) �̈ʒu����
 * RaceObserver �������������[�X�`��N���X�ɒʒm���邽�߂̃C�x���g�I�u�W�F�N�g�N���X
 * @author   : Junpei TSUJI (DT&T)
 * @version  : 1.0.0-2011.12.25
 */
class UpdateEvent
{
public:
	// �R���X�g���N�^
	UpdateEvent(){ controllKey = 0; }
	// �f�X�g���N�^
	~UpdateEvent(){}

	// �C�x���g��������
	tm        *time;
	// ���݂̃R�[�X�摜
	IplImage  *image;
	// �R�[�X�N���X�̃C���X�^���X
	Course    *course;

	// ���݃}�V�����ʉߒ��̃`�F�b�N�|�C���g�̃C���f�b�N�X��, �e�}�V���ɑ΂��Ă܂Ƃ߂��z��
	std::vector<int>         *currentCheckPointIndexArray;
	// ���݃}�V���̍��W��, �e�}�V���ɑ΂��Ă܂Ƃ߂��z��
	std::vector<CvPoint>     *currentPositionArray;
	// ���݃}�V���̐i�s�����̃x�N�g����, �e�}�V���ɑ΂��Ă܂Ƃ߂��z��
	std::vector<cv::Point2d> *currentDirectionArray;

	// �L�[�{�[�h���͂̐���L�[ (�C�x���g�������ɑ΂��Ēʒm����p)
	int controllKey;
};


/**
 * �g���b�L���O�I�u�W�F�N�g (�}�V��) �̋��E�ʉ߂�
 * MachineObserver�����������}�V������p�N���X�ɒʒm���邽�߂̃C�x���g�I�u�W�F�N�g�N���X
 * @author   : Junpei TSUJI (DT&T)
 * @version  : 1.0.0-2011.12.25
 */
class OutOfBoundEvent
{
public:
	// �R���X�g���N�^
	OutOfBoundEvent(){}
	// �f�X�g���N�^
	~OutOfBoundEvent(){}

	// ���E�ɓ��������Ƃ�\���萔
	static const int OUT_OF_BOUND_FLAG_IN = 1; 
	// ���E����o�����Ƃ�\���萔
	static const int OUT_OF_BOUND_FLAG_OUT = -1; 
	// ���E�̓��o�Ɋւ����񂪕s���̏ꍇ��\���萔
	static const int OUT_OF_BOUND_FLAG_UNKNOWN = 0; 

	// ���ݒʉ߂������E���u�k���v�ł��邱�Ƃ�\���萔
	static const int CURRENT_BOUND_EDHE_NORTH = 0; 
	// ���ݒʉ߂������E���u�����v�ł��邱�Ƃ�\���萔
	static const int CURRENT_BOUND_EDHE_EAST  = 1; 
	// ���ݒʉ߂������E���u�쑤�v�ł��邱�Ƃ�\���萔
	static const int CURRENT_BOUND_EDHE_SOUTH = 2; 
	// ���ݒʉ߂������E���u�����v�ł��邱�Ƃ�\���萔
	static const int CURRENT_BOUND_EDHE_WEST  = 3;

	// ���E�̓��o��\���ϐ�
	int outOfBoundFlag;
	// ���ݒʉ߂������E��\���ϐ�
	int currentBoundEdge;
};


/**
 * �g���b�L���O�I�u�W�F�N�g (�}�V��) �̃R�[�X�ɑ΂�����W����
 * MachineObserver�����������}�V������p�N���X�ɒʒm���邽�߂̃C�x���g�I�u�W�F�N�g�N���X
 * @author   : Junpei TSUJI (DT&T)
 * @version  : 1.0.0-2011.12.25
 */
class ControllDataEvent
{
public:
	// �R���X�g���N�^
	ControllDataEvent(){ controllKey = 0; }

	// �L�[�{�[�h���͂̐���L�[ (�C�x���g�������ɑ΂��Ēʒm����p)
	int             controllKey;
	// �R�[�X���̃C�e���[�^
	CourseIterator *iterator;    
	// �}�V���̌��݈ʒu
	CvPoint         currentPoint;     
	// �}�V���̌��݂̕���
	cv::Point2d     currentDirection; 
};

