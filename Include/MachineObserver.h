#pragma once
#include "Event.h"

/**
 * �}�V���P�̂̏�Ԃ��Ď����邽�߂̃I�u�U�[�o�[�N���X
 * @author   : Junpei TSUJI (DT&T)
 * @version  : 1.0.0-2011.12.25
 */
class MachineObserver 
{
public:
	/**
	 * @bried    : �g���b�L���O�I�u�W�F�N�g�����E�𒴂������ɋN������C�x���g�n���h��
	 * @pram[in] : OutOfBoundEvent�^�̃C�x���g�I�u�W�F�N�g
	 * @return   : �Ȃ�
	 */
	virtual void outOfBoundEventHandler(OutOfBoundEvent *e)=0;

	/**
	 * @bried    : �R�[�X�Ƃ̍��W���󂯎���ċN������C�x���g�n���h��
	 * @pram[in] : ControllDataEvent�^�̃C�x���g�I�u�W�F�N�g
	 * @return   : �Ȃ�
	 */
	virtual void controllDataEventHandler(ControllDataEvent *e)=0;	

};


