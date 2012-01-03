#pragma once

#include "MachineObserver.h"
#include "Event.h"
#include "SerialPort.h"


/**
 * �}�V�����V���A���ʐM�ő��삷�邽�߂̃N���X
 * @author   : Junpei TSUJI (DT&T)
 * @version  : 1.0.0-2011.12.25
 */
class MachineController : public MachineObserver, public serial::SerialObserver
{
public:
	/**
	 * @bried    : �R���X�g���N�^
	 * @pram[in] : COM�|�[�g�̕�����ƃ{�[���[�g�̐��l
	 */
	MachineController(std::string comPort, int baudRate);

	/**
	 * @bried    : �f�X�g���N�^
	 */ 
	~MachineController();

	/**
	 * @bried    : �V���A���ʐM����M����C�x���g�n���h��
	 * @pram[in] : ��M���镶����
	 * @return   : �Ȃ�
	 */ 
	void notify( const std::string& str );

	/**
	 * @bried    : �g���b�L���O�I�u�W�F�N�g�����E�𒴂������ɋN������C�x���g�n���h��
	 * @pram[in] : OutOfBoundEvent�^�̃C�x���g�I�u�W�F�N�g
	 * @return   : �Ȃ�
	 */
	void outOfBoundEventHandler(OutOfBoundEvent *e);

	/**
	 * @bried    : �R�[�X�Ƃ̑��΍��W���󂯎���ċN������C�x���g�n���h��
	 * @pram[in] : ControllDataEvent�^�̃C�x���g�I�u�W�F�N�g
	 * @return   : �Ȃ�
	 */
	void controllDataEventHandler(ControllDataEvent *e);	

protected:
	/**
	 * @bried    : �V���A���ʐM�̃C���X�^���X
	 */
	serial::SerialPort *serial;  
};
