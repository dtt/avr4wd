#pragma once

#include "RaceObserver.h"
#include "Event.h"


/**
 * ���[�X����
 * @author   : Junpei TSUJI (DT&T)
 * @version  : 1.0.0-2011.12.25
 */
class RaceViewer : public RaceObserver
{
public:
	/**
	 * @bried    : �R���X�g���N�^
	 * @pram[in] : �Ȃ�
	 */
	RaceViewer();
	/**
	 * @bried    : �f�X�g���N�^
	 * @pram[in] : �Ȃ�
	 */
	~RaceViewer();

	/**
	 * @bried    : �ʒu�̍X�V���ϑ������Ƃ��ɋN������C�x���g�n���h��
	 * @pram[in] : UpdateEvent�^�̃C�x���g�I�u�W�F�N�g
	 * @return   : �Ȃ�
	 */
	void updateEventHandler(UpdateEvent *e);

	/**
	 * @bried    : IplImage�ɃR�[�X���������ރ��\�b�h
	 * @pram[in] : IplImage, Course �^�̃I�u�W�F�N�g
	 * @return   : �Ȃ�
	 */
	void drawCourse(IplImage *image, Course *course);
	
};
