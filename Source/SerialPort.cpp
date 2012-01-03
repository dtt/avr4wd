/**
 * http://chicklab.blog84.fc2.com/page-1.html ������ Junpei Tsuji (DT&T) ������
 */


#include <list>
#include <iostream>
#include <algorithm>
#include <queue>
 
#include <boost/foreach.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/smart_ptr.hpp>
 
#include "SerialPort.h"
 
using namespace serial;
using namespace boost::asio;
using namespace std;
 
/// <summary>
/// boost�̃C���^�[�t�F�[�X���B�����邽�߂̃N���X
/// </summary>
class SerialPort::serial_impl
{
// �R���X�g���N�^�A�f�X�g���N�^----------------------------
public:
    serial_impl()
        : serial_port( NULL )
        , br( serial_port_base::baud_rate(9600) )
        , cs( serial_port_base::character_size( 8 ) )
        , fc( serial_port_base::flow_control::none )
        , parity( serial_port_base::parity::none )
        , sb( serial_port_base::stop_bits::one )
        , com( "COM1" )
    {}
 
    virtual ~serial_impl()
    {
        if( serial_port ) delete serial_port; serial_port = NULL;
    }
 
 
// ����----------------------------------------------------
public:
    // shared_ptr
    std::vector< SerialObserver* > ptrList;
 
    // thread
    boost::thread ioThread;
    boost::condition recv_condition;
    boost::mutex recv_sync;
 
    // �V���A���̐ݒ�n��
    io_service io;
    std::string com;
    serial_port *serial_port;
    serial_port_base::baud_rate br;
    serial_port_base::character_size cs;
    serial_port_base::flow_control::type fc;
    serial_port_base::parity::type parity;
    serial_port_base::stop_bits::type sb;
};
 
 
/**
* @brief �R���X�g���N�^
*/
SerialPort::SerialPort()
: impl( new serial_impl() )
, is_connect_( false )
{}
 
 
/**
* @brief : �f�X�g���N�^
*/
SerialPort::~SerialPort() {
    close();
}
 
 
/**
* @brief        : �|�[�g�̃I�[�v��
* @param[in]    : com�|�[�g
* @param[in]    : 1�o�C�g�̃r�b�g��
* @param[in]    : �p���e�B���w��
* @param[in]    : �X�g�b�v�r�b�g�w��
* @return       : ��������
*/
bool SerialPort::open(
        const std::string &com,
        int baudrate,
        int cs,
        int parity,
        int stopbits,
        int flow
    )
{
    if( is_connect_ ) return false;
 
    boost::system::error_code ret;
 
    // �|�[�g�̃I�[�v��
    impl->serial_port = new serial_port( impl->io );
    impl->serial_port->open( com, ret );
 
    if( ret ){
        std::cerr << "resial_port open() error " << ret << std::endl;
        return false;
    }
 
    // �ڑ��t���O
    is_connect_ = true;
 
    // �p���e�B�l�̐ݒ�
    serial_port_base::parity::type parity_t = serial_port_base::parity::none;
    if( parity == Parity::Even ) parity_t = serial_port_base::parity::even;
    else if( parity == Parity::Odd ) parity_t = serial_port_base::parity::odd;
 
    // Stop Bists
    serial_port_base::stop_bits::type stopbit_t = serial_port_base::stop_bits::one;
    if( stopbits == StopBits::Two ) stopbit_t = serial_port_base::stop_bits::two;
    else if( stopbits == StopBits::OnePointFive ) stopbit_t = serial_port_base::stop_bits::onepointfive;
 
    // flow control
    serial_port_base::flow_control::type flow_t = serial_port_base::flow_control::none;
    if( flow == FlowControl::Hardware ) flow_t = serial_port_base::flow_control::hardware;
    else if( flow == FlowControl::Software ) flow_t = serial_port_base::flow_control::software;
 
    // �ݒ�l�̎擾
    impl->com = com;
    impl->br = serial_port_base::baud_rate( baudrate );
    impl->cs = serial_port_base::character_size( cs );
    impl->parity = parity_t;
    impl->sb = stopbit_t;
    impl->fc = flow_t;
 
    impl->serial_port->set_option( impl->br );
    impl->serial_port->set_option( serial_port_base::parity(parity_t) );
    impl->serial_port->set_option( serial_port_base::character_size( cs ) );
    impl->serial_port->set_option( serial_port_base::stop_bits(stopbit_t) );
    impl->serial_port->set_option( serial_port_base::flow_control(flow_t) );
 
    // �ǂݍ��ݗp�̊֐���ݒ�
    impl->serial_port->async_read_some(
        boost::asio::buffer(rBuffer, 1024),
        boost::bind(&SerialPort::read_ok, this, _1, _2) );
 
    // IO�T�[�r�X�̊J�n
    impl->ioThread = boost::thread( boost::bind(&boost::asio::io_service::run, &impl->io) );
 
    return true;
}
 
 
/**
* @brier     : �I�u�W�F�N�g�̓o�^���s��
* @param[in] : �o�^���s���I�u�W�F�N�g
* @return    : ��������
*/
bool SerialPort::attach(SerialObserver *ob) {
    std::vector<SerialObserver*>::iterator it = std::find( impl->ptrList.begin(), impl->ptrList.end(), ob );
 
    // �o�^����Ă��Ȃ�������A�I�u�U�[�o�[��o�^
    if( it != impl->ptrList.end() ) return false;
    impl->ptrList.push_back(ob);
    return true;
}
 
 
/**
* @brier     : �I�u�W�F�N�g�̔j�����s��
* @param[in] : �j�����s���I�u�W�F�N�g
* @return    : ��������
*/
bool SerialPort::detach(SerialObserver *ob) {
    std::vector<SerialObserver*>::iterator it = std::find( impl->ptrList.begin(), impl->ptrList.end(), ob );
     
    // �o�^����Ă��Ȃ�������A�I�u�U�[�o�[��o�^
    if( it == impl->ptrList.end() ) return false;
    impl->ptrList.erase( it );
    return true;
}
 
 
/**
* @brief    : ��Ԃ̍X�V��ʒm����
* @param[in]: ��M������
*/
void SerialPort::notifyAll( const std::string& str ) {
    // �S�ẴI�u�U�[�o�[�ɒʒm
    BOOST_FOREACH( SerialObserver* ob, impl->ptrList ) ob->notify(str);
 
    // �R���f�B�V��������
    boost::mutex::scoped_lock lk( impl->recv_sync );
    readData = str;
    impl->recv_condition.notify_all();
}
 
 
/**
* @brief    : �|�[�g�̃N���[�Y
* @return   : ��������
*/
bool SerialPort::close()
{
    if( !is_connect_ ) return false;
    impl->recv_condition.notify_all();
    impl->serial_port->close();
    is_connect_ = false;
    return true;
}
 
 
/*
* @brief    �F �f�[�^���[�h
* @return   �F ��������
* @param[out]�F ��M�f�[�^
* @param[in] : �^�C���A�E�g[ms]
*/
bool SerialPort::receive( std::string& str, double timeout ) {
     
    // �ڑ�����
    if( !is_connect_ ) return false;
 
    boost::mutex::scoped_lock lk( impl->recv_sync );
 
    // ��M�҂�
    boost::xtime xt;
    boost::xtime_get(&xt, boost::TIME_UTC);
    xt.nsec += static_cast<int>(timeout*1000.0*1000.0);
 
    // ��M�҂����s
    if( !impl->recv_condition.timed_wait(lk,xt) ) return false;
     
    // ��M��������i�[
    str = this->readData;
    return true;
}
 
 
/**
/* @brief   : ������̑��M�֐�
/* @return  : ��������
*/
bool SerialPort::send( const std::string& s ) {
    return write( s.c_str(), s.size() );
}
 
bool SerialPort::send( char c ) {
    return write( &c, 1 );
}
 
bool SerialPort::send( const char* c, int size ) {
    return write( c, size );
}
 
bool SerialPort::write( const char* str, int n ) {
    boost::system::error_code ret;
    if( !is_connect_ ) return false;
 
    impl->serial_port->write_some( boost::asio::buffer( str, n ), ret );
 
    if( ret ) {
        std::cerr << "serial_port::write_some() return = " << ret << std::endl;
        return false;
    }
 
    return true;
}
 
void SerialPort::read_ok( const boost::system::error_code& e, size_t size ) {
    if( e ) {
        std::cerr << "read_some() Error = " << e << std::endl;
        return;
    }
 
    // ��M����
    std::string str( rBuffer, rBuffer+size );  
    //readQue.push_back( str );
 
    // �X�V����
    notifyAll( str );
 
    // �ǂݍ��݂����������̂ŁA�ēx�ݒ�
    impl->serial_port->async_read_some(
        boost::asio::buffer(rBuffer, 1024),
        boost::bind(&SerialPort::read_ok, this, _1, _2) );
}
