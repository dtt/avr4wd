
#include "MachineController.h"
#include <iostream>


/**
 * �O���[�o���ϐ� (Machine Controller�݂̂Ŏg�p)
 */
char txBuf[40];      // ���M�p������o�b�t�@
char rxBuf[40];      // ��M�p������o�b�t�@


/**
 * PID�̐���ʂ��擾����֐�
 */
double getPIDControlValue(double deviation){
	// ��ᐧ��̃p�����[�^
	double Kp = 1.5;

	// �����
	double input = - Kp * deviation;

	// ����ʂ��T�[�{�̊p�x�l�ɒ���
	input = (input * 180.0) + 90.0;
	// �[��؂�
	if( input > 179.0 ){
		input = 179.0;
	}else if( input < 0.0 ){
		input = 0.0;
	}
	return input;
}



/**
 * @bried    : �R���X�g���N�^
 * @pram[in] : COM�|�[�g�̕�����ƃ{�[���[�g�̐��l
 */
MachineController::MachineController(std::string comPort, int baudRate){

	// �V���A���C���X�^���X���쐬
	std::cout << "Serial instance has been created. " << std::endl;
	serial = new serial::SerialPort();

	// �|�[�g�I�[�v��
	std::cout << "Port " << comPort << " has been opened. Baud Rate is " << baudRate << "." <<  std::endl;
	serial->open( comPort, baudRate );

	// �I�u�U�[�o�[�Ƃ��Ė{�C���X�^���X���V���A���ɓo�^
	serial->attach( this );

	// �V���A�����J�n
	std::cout << "Start communication." << std::endl;
}

/**
 * @bried    : �f�X�g���N�^
 */
MachineController::~MachineController(){
	delete serial;
}


/**
 * @bried    : �V���A���ʐM����M����C�x���g�n���h��
 * @pram[in] : ��M���镶����
 * @return   : �Ȃ�
 */ 
void MachineController::notify( const std::string& str ) {
	std::cout << "serial>" << str;
}


/**
 * @bried    : �g���b�L���O�I�u�W�F�N�g�����E�𒴂������ɋN������C�x���g�n���h��
 * @pram[in] : OutOfBoundEvent�^�̃C�x���g�I�u�W�F�N�g
 * @return   : �Ȃ�
 */
void MachineController::outOfBoundEventHandler(OutOfBoundEvent *e){
	std::cout << "outOfBoundEventHandler> " << std::endl;
}

/**
 * @bried    : �R�[�X�Ƃ̑��΍��W���󂯎���ċN������C�x���g�n���h��
 * @pram[in] : RelativeCoordinateEvent�^�̃C�x���g�I�u�W�F�N�g
 * @return   : �Ȃ�
 */
void MachineController::controllDataEventHandler(ControllDataEvent *e){
	std::cout << "controllDataEventHandler> " <<std::endl;

	
	if(e->controllKey == 's'){			// ��~
		std::cout << "stop machine!" << std::endl;
		this->serial->send("#ST0\n");
	}
	else if(e->controllKey == 'r'){		// �ĊJ
		std::cout << "restart machine!" << std::endl;
		this->serial->send("#RS0\n");
	}

	// �R�[�X�̍��W���擾
	CvPoint currentCheckPoint  = *(e->iterator->getNextPoint());  // ���ݒʉ߂����R�[�X�x�N�g���̍��W
	CvPoint targetPoint  = *(e->iterator->getNextPoint());        // ���̎��̃R�[�X�x�N�g���̍��W�i�ڕW�n�_�j
	
	// ���݂̃}�V���ʒu�̎擾
	CvPoint currentPoint = e->currentPoint;

	// ���݂̃}�V�������̎擾
	double dx = e->currentDirection.x;
	double dy = e->currentDirection.y;

	// �^�[�Q�b�g�̃`�F�b�N�|�C���g�̕���
	double tx = targetPoint.x - currentPoint.x;
	double ty = targetPoint.y - currentPoint.y;

	// �e�x�N�g���̃m�������v�Z
	double squaredNormT = (tx * tx) + (ty * ty);
	double squaredNormD = (dx * dx) + (dy * dy);
	
	// �m�����̑傫�������̏ꍇ
	if(squaredNormT > 0 && squaredNormD > 0){
		
		// ���ς���x�N�g���̂Ȃ��p�i��Βl�j���v�Z
		double dotProduct = (dx * tx) + (dy * ty);
		double cosValue = dotProduct / (sqrt(squaredNormT) * sqrt(squaredNormD));
		double theta = acos( cosValue );
		
		// �p�x�̐�������ɊO�ς��v�Z
		double outerProduct = (dx * ty) - (dy * tx);
		if(  outerProduct > 0 ){
			// ����
			theta = theta;
		}
		else{
			// �E��
			theta = -theta;
		}

		// �΍� [-1:1] = [�E��:����]
		double deviation = theta / 3.14159;

		// PID�̐���ʂ��擾
		int floorInput = (int)::getPIDControlValue(deviation);

		// ����ʂ�ʒm�p������ɕϊ�
		sprintf(txBuf, "#TR%03d\n", floorInput);

		// �V���A���o�R�Ń}�V���ɒʒm
		this->serial->send(txBuf);
		std::string txString = std::string(txBuf);
		std::cout << txString;

	}
}
