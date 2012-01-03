
#include "MachineController.h"
#include <iostream>


/**
 * グローバル変数 (Machine Controllerのみで使用)
 */
char txBuf[40];      // 送信用文字列バッファ
char rxBuf[40];      // 受信用文字列バッファ


/**
 * PIDの制御量を取得する関数
 */
double getPIDControlValue(double deviation){
	// 比例制御のパラメータ
	double Kp = 1.5;

	// 制御量
	double input = - Kp * deviation;

	// 制御量をサーボの角度値に直す
	input = (input * 180.0) + 90.0;
	// 端を切る
	if( input > 179.0 ){
		input = 179.0;
	}else if( input < 0.0 ){
		input = 0.0;
	}
	return input;
}



/**
 * @bried    : コンストラクタ
 * @pram[in] : COMポートの文字列とボーレートの数値
 */
MachineController::MachineController(std::string comPort, int baudRate){

	// シリアルインスタンスを作成
	std::cout << "Serial instance has been created. " << std::endl;
	serial = new serial::SerialPort();

	// ポートオープン
	std::cout << "Port " << comPort << " has been opened. Baud Rate is " << baudRate << "." <<  std::endl;
	serial->open( comPort, baudRate );

	// オブザーバーとして本インスタンスをシリアルに登録
	serial->attach( this );

	// シリアルを開始
	std::cout << "Start communication." << std::endl;
}

/**
 * @bried    : デストラクタ
 */
MachineController::~MachineController(){
	delete serial;
}


/**
 * @bried    : シリアル通信を受信するイベントハンドラ
 * @pram[in] : 受信する文字列
 * @return   : なし
 */ 
void MachineController::notify( const std::string& str ) {
	std::cout << "serial>" << str;
}


/**
 * @bried    : トラッキングオブジェクトが境界を超えた時に起動するイベントハンドラ
 * @pram[in] : OutOfBoundEvent型のイベントオブジェクト
 * @return   : なし
 */
void MachineController::outOfBoundEventHandler(OutOfBoundEvent *e){
	std::cout << "outOfBoundEventHandler> " << std::endl;
}

/**
 * @bried    : コースとの相対座標を受け取って起動するイベントハンドラ
 * @pram[in] : RelativeCoordinateEvent型のイベントオブジェクト
 * @return   : なし
 */
void MachineController::controllDataEventHandler(ControllDataEvent *e){
	std::cout << "controllDataEventHandler> " <<std::endl;

	
	if(e->controllKey == 's'){			// 停止
		std::cout << "stop machine!" << std::endl;
		this->serial->send("#ST0\n");
	}
	else if(e->controllKey == 'r'){		// 再開
		std::cout << "restart machine!" << std::endl;
		this->serial->send("#RS0\n");
	}

	// コースの座標を取得
	CvPoint currentCheckPoint  = *(e->iterator->getNextPoint());  // 現在通過したコースベクトルの座標
	CvPoint targetPoint  = *(e->iterator->getNextPoint());        // その次のコースベクトルの座標（目標地点）
	
	// 現在のマシン位置の取得
	CvPoint currentPoint = e->currentPoint;

	// 現在のマシン方向の取得
	double dx = e->currentDirection.x;
	double dy = e->currentDirection.y;

	// ターゲットのチェックポイントの方向
	double tx = targetPoint.x - currentPoint.x;
	double ty = targetPoint.y - currentPoint.y;

	// 各ベクトルのノルムを計算
	double squaredNormT = (tx * tx) + (ty * ty);
	double squaredNormD = (dx * dx) + (dy * dy);
	
	// ノルムの大きさが正の場合
	if(squaredNormT > 0 && squaredNormD > 0){
		
		// 内積からベクトルのなす角（絶対値）を計算
		double dotProduct = (dx * tx) + (dy * ty);
		double cosValue = dotProduct / (sqrt(squaredNormT) * sqrt(squaredNormD));
		double theta = acos( cosValue );
		
		// 角度の正負判定に外積を計算
		double outerProduct = (dx * ty) - (dy * tx);
		if(  outerProduct > 0 ){
			// 左折
			theta = theta;
		}
		else{
			// 右折
			theta = -theta;
		}

		// 偏差 [-1:1] = [右折:左折]
		double deviation = theta / 3.14159;

		// PIDの制御量を取得
		int floorInput = (int)::getPIDControlValue(deviation);

		// 制御量を通知用文字列に変換
		sprintf(txBuf, "#TR%03d\n", floorInput);

		// シリアル経由でマシンに通知
		this->serial->send(txBuf);
		std::string txString = std::string(txBuf);
		std::cout << txString;

	}
}
