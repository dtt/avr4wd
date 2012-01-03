#pragma once

#include "Event.h"
#include "RaceObserver.h"
#include "MachineObserver.h"

/**
 * ���[�X�����Ǘ�����N���X�̃C���^�t�F�[�X
 * @author   : Junpei TSUJI (DT&T)
 * @version  : 1.0.0-2011.12.25
 */
class Race
{
public:
	/**
	 * @brief     : RaceObserver��ǉ�����֐�
	 * @param[in] : RaceObserver�N���X
	 * @return    : ��������
	 */
	virtual bool attachRaceObserver( RaceObserver* ob )=0;

	/**
	 * @brief    : RaceObserver���폜����֐�
	 * @pram[in] : RaceObserver�N���X
	 * @return   : ��������
	 */
	virtual bool detachRaceObserver( RaceObserver* ob )=0;

	/**
	 * @brief     : MachineObserver��ǉ�����֐�
	 * @param[in] : MachineObserver�N���X
	 * @return    : ��������
	 */
	virtual bool attachMachineObserver( MachineObserver* ob )=0;

	/**
	 * @brief    : MachineObserver���폜����֐�
	 * @pram[in] : MachineObserver�N���X
	 * @return   : ��������
	 */
	virtual bool detachMachineObserver( MachineObserver* ob )=0;

	/**
	 * @bried    : RaceObserver��updateEventHandler���Ăяo���֐�
	 * @pram[in] : UpdateEvent�^�̃C�x���g�I�u�W�F�N�g
	 * @return   : �Ȃ�
	 */
	virtual void updateNotify(UpdateEvent *e)=0;

	/**
	 * @bried    : MachineObserver��outOfBoundEventHandler���Ăяo���֐�
	 * @pram[in] : OutOfBoundEvent�^�̃C�x���g�I�u�W�F�N�g
	 * @return   : �Ȃ�
	 */
	virtual void outOfBoundNotify(MachineObserver* ob, OutOfBoundEvent *e)=0;

	/**
	 * @bried    : MachineObserver��controllDataEventHandler���Ăяo���֐�
	 * @pram[in] : ControllDataEvent�^�̃C�x���g�I�u�W�F�N�g
	 * @return   : �Ȃ�
	 */
	virtual void controllDataNotifyAll(ControllDataEvent *e)=0;


	virtual void startRaceTracking()=0;

};

