#pragma once
#include "Event.h"


/**
 * ���[�X���Ď�����I�u�U�[�o�[�N���X�̃C���^�t�F�[�X
 * @author   : Junpei TSUJI (DT&T)
 * @version  : 1.0.0-2011.12.25
 */
class RaceObserver 
{
public:
	/**
	 * @bried    : �ʒu�̍X�V���ϑ������Ƃ��ɋN������C�x���g�n���h��
	 * @pram[in] : UpdateEvent�^�̃C�x���g�I�u�W�F�N�g
	 * @return   : �Ȃ�
	 */
	virtual void updateEventHandler(UpdateEvent *e)=0;

};

