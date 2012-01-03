#pragma once

/**
 * http://chicklab.blog84.fc2.com/page-1.html ������ Junpei Tsuji (DT&T) ������
 */


#include <string>
#include <queue>
#include <boost/smart_ptr.hpp>

namespace boost{ namespace system{ class error_code; }; };

namespace serial
{
	/**
	* @brief�F�V���A���|�[�g�̃I�v�V������ݒ肷�邽�߂�Enum�N���X�Q
	*/
	// �{�[���[�g�ݒ�p
	class BaudRate {
	public:
		enum {
			B2400 = 2400,
			B4800 = 4800,
			B9600 = 9600,
			B19200 = 19200,
			B38400 = 38400,
			B115200 = 115200
		};
	};

	// �p���e�B�ݒ�p
	class Parity {
	public:
		enum {
			None=0,
			Odd,
			Even
		};
	};

	// �X�g�b�v�r�b�g�ݒ�p
	class StopBits {
	public:
		enum {
			One=0,
			OnePointFive,
			Two
		};
	};

	// �f�[�^�̃o�C�g�T�C�Y�ݒ�p
	class ByteSize{
	public:
		enum{
			B7 = 7,
			B8 = 8
		};
	};

	// �t���[�R���g���[���p�̐ݒ�
	class FlowControl{
	public:
		enum{
			None=0,
			Hardware,
			Software
		};
	};

	class SerialPort;
	/**
	/* @brief  : observer���g���ăR�[���o�b�N���������邽�߂̃C���^�[�t�F�[�X�N���X
	*/
	class SerialObserver
	{
		friend SerialPort;
	public:
		SerialObserver(){}
		virtual ~SerialObserver(){}

	protected:
		virtual void notify( const std::string& str )=0;

	};


	/**
	* @brief    : boost���g�����V���A���ʐM�N���X
	*             �C�x���g�œ����悤�ɂ����̂���
	*           �@�ʐM�ɗ]���ȃ��\�[�X�������K�v���Ȃ��Ȃ�܂��B
	*/
	class SerialPort
	{
		// �R���X�g���N�^�A�f�X�g���N�^------------------------
	public:
		SerialPort();
		virtual ~SerialPort();


		// ����------------------------------------------------
	public:
		/*
		* @brief        : �|�[�g�̃I�[�v��
		* @param[in]    : com�|�[�g
		* @param[in]    : 1�o�C�g�̃r�b�g��
		* @param[in]    : �p���e�B���w��
		* @param[in]    : �X�g�b�v�r�b�g�w��
		* @return       : ��������
		*/
		bool open(
			const std::string& com = "COM1",
			int baudrate = BaudRate::B115200,
			int bytesize = ByteSize::B8,
			int parity = Parity::None,
			int stopbits = StopBits::One,
			int flowcontrol = FlowControl::None
			);

		/**
		* @brief    : �|�[�g�̃N���[�Y
		* @return   : ��������
		*/
		bool close();

		/**
		* @brief     : ������𑗐M����֐�
		* @param[in] : ���M���镶����
		*/
		bool send( const std::string& s );

		/**
		* @brief     : ������𑗐M����֐�
		* @param[in] : ���M���镶��
		*/
		bool send( char c );

		/**
		* @brief     : ������𑗐M����֐�
		* @param[in] : ���M���镶����
		* @param[in] : ������̃T�C�Y
		*/
		bool send( const char *c, int size );

		/**
		* @brief     : observer��ǉ�����֐�
		* @param[in] : observer�N���X
		* @return    : ��������
		*/
		bool attach( SerialObserver* ob );

		/**
		/* @brief    : observer���폜����֐�
		/* @pram[in] : observer�N���X
		/* @return   : ��������
		*/
		bool detach( SerialObserver* ob );

		/**
		/* @brief      : �f�[�^����M����֐�
		/* @param[out] : �擾�f�[�^
		/* @return     : �c��̃f�[�^��
		*/
		bool receive( std::string& str, double timeout );

		/**
		* @brief    : �ڑ��m�F
		* @return   : �ڑ���
		*/
		bool isConnect() const { return is_connect_; }



	private:
		// �X�V�֐�
		virtual void notifyAll( const std::string& str );

		// �f�[�^��������
		bool write( const char* str, int n );


		// ����------------------------------------------------
	private:
		class serial_impl;
		boost::shared_ptr<serial_impl> impl;

		// ��M�p�o�b�t�@
		char rBuffer[1024];
		int readSize;

		void read_ok( const boost::system::error_code& e, size_t size );
		bool is_connect_;

		// �ŐV�ł̎�M�f�[�^
		std::string readData;
	};

};

