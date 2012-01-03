/**
 * http://chicklab.blog84.fc2.com/page-1.html を元に Junpei Tsuji (DT&T) が改変
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
/// boostのインターフェースを隠蔽するためのクラス
/// </summary>
class SerialPort::serial_impl
{
// コンストラクタ、デストラクタ----------------------------
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
 
 
// 属性----------------------------------------------------
public:
    // shared_ptr
    std::vector< SerialObserver* > ptrList;
 
    // thread
    boost::thread ioThread;
    boost::condition recv_condition;
    boost::mutex recv_sync;
 
    // シリアルの設定系統
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
* @brief コンストラクタ
*/
SerialPort::SerialPort()
: impl( new serial_impl() )
, is_connect_( false )
{}
 
 
/**
* @brief : デストラクタ
*/
SerialPort::~SerialPort() {
    close();
}
 
 
/**
* @brief        : ポートのオープン
* @param[in]    : comポート
* @param[in]    : 1バイトのビット数
* @param[in]    : パリティを指定
* @param[in]    : ストップビット指定
* @return       : 成功判定
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
 
    // ポートのオープン
    impl->serial_port = new serial_port( impl->io );
    impl->serial_port->open( com, ret );
 
    if( ret ){
        std::cerr << "resial_port open() error " << ret << std::endl;
        return false;
    }
 
    // 接続フラグ
    is_connect_ = true;
 
    // パリティ値の設定
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
 
    // 設定値の取得
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
 
    // 読み込み用の関数を設定
    impl->serial_port->async_read_some(
        boost::asio::buffer(rBuffer, 1024),
        boost::bind(&SerialPort::read_ok, this, _1, _2) );
 
    // IOサービスの開始
    impl->ioThread = boost::thread( boost::bind(&boost::asio::io_service::run, &impl->io) );
 
    return true;
}
 
 
/**
* @brier     : オブジェクトの登録を行う
* @param[in] : 登録を行うオブジェクト
* @return    : 成功判定
*/
bool SerialPort::attach(SerialObserver *ob) {
    std::vector<SerialObserver*>::iterator it = std::find( impl->ptrList.begin(), impl->ptrList.end(), ob );
 
    // 登録されていなかったら、オブザーバーを登録
    if( it != impl->ptrList.end() ) return false;
    impl->ptrList.push_back(ob);
    return true;
}
 
 
/**
* @brier     : オブジェクトの破棄を行う
* @param[in] : 破棄を行うオブジェクト
* @return    : 成功判定
*/
bool SerialPort::detach(SerialObserver *ob) {
    std::vector<SerialObserver*>::iterator it = std::find( impl->ptrList.begin(), impl->ptrList.end(), ob );
     
    // 登録されていなかったら、オブザーバーを登録
    if( it == impl->ptrList.end() ) return false;
    impl->ptrList.erase( it );
    return true;
}
 
 
/**
* @brief    : 状態の更新を通知する
* @param[in]: 受信文字列
*/
void SerialPort::notifyAll( const std::string& str ) {
    // 全てのオブザーバーに通知
    BOOST_FOREACH( SerialObserver* ob, impl->ptrList ) ob->notify(str);
 
    // コンディション解除
    boost::mutex::scoped_lock lk( impl->recv_sync );
    readData = str;
    impl->recv_condition.notify_all();
}
 
 
/**
* @brief    : ポートのクローズ
* @return   : 成功判定
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
* @brief    ： データリード
* @return   ： 成功判定
* @param[out]： 受信データ
* @param[in] : タイムアウト[ms]
*/
bool SerialPort::receive( std::string& str, double timeout ) {
     
    // 接続判定
    if( !is_connect_ ) return false;
 
    boost::mutex::scoped_lock lk( impl->recv_sync );
 
    // 受信待ち
    boost::xtime xt;
    boost::xtime_get(&xt, boost::TIME_UTC);
    xt.nsec += static_cast<int>(timeout*1000.0*1000.0);
 
    // 受信待ち失敗
    if( !impl->recv_condition.timed_wait(lk,xt) ) return false;
     
    // 受信文字列を格納
    str = this->readData;
    return true;
}
 
 
/**
/* @brief   : 文字列の送信関数
/* @return  : 成功判定
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
 
    // 受信処理
    std::string str( rBuffer, rBuffer+size );  
    //readQue.push_back( str );
 
    // 更新処理
    notifyAll( str );
 
    // 読み込みが完了したので、再度設定
    impl->serial_port->async_read_some(
        boost::asio::buffer(rBuffer, 1024),
        boost::bind(&SerialPort::read_ok, this, _1, _2) );
}
