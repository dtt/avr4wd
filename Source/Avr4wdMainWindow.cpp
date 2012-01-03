#include "Avr4wdMainWindow.h"

Avr4wdMainWindow::Avr4wdMainWindow(){

	this->hasFlag = false;
	
	root = new Ogre::Root("plugins.cfg","Ogre.cfg");
	root->showConfigDialog();

	Ogre::ConfigFile* cf = new Ogre::ConfigFile();

	cf->load("resources.cfg");

	Ogre::ConfigFile::SectionIterator seci = cf->getSectionIterator();
	std::string secName,typeName,archName;
	while(seci.hasMoreElements()){
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap* settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for(i = settings->begin();i!=settings->end();++i){
			typeName = i->first;
			archName = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName,typeName,secName);
		}
	}

	Ogre::RenderWindow* window = root->initialise(true,"Race Window");
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	sceneMgr = root->createSceneManager(Ogre::ST_GENERIC,"SceneManager");
	avr4wdFrameListener = new Avr4wdFrameListener(this->sceneMgr,window);

	root->addFrameListener(avr4wdFrameListener);

	
	//root->startRendering();
	//root->renderOneFrame();
}

Avr4wdMainWindow::~Avr4wdMainWindow(){
	
}

void Avr4wdMainWindow::updateEventHandler(UpdateEvent *e){

	// マシンの位置を受信
	CvPoint     point     = e->currentPositionArray->at(0);
	cv::Point2d direction = e->currentDirectionArray->at(0);
	int checkPointIndex   = e->currentCheckPointIndexArray->at(0);
	
	if(e->image != NULL && e->course != NULL){
		if(!hasFlag){
			hasFlag = this->avr4wdFrameListener->setCourseFlag(e->course,e->course->getImageSize().width,e->course->getImageSize().height);
		}
		// コースを書き込み
		this->drawCourse(e->image, e->course);

		// マシンの方向を画像に追加
		cvCircle (e->image, point, 10, CV_RGB (255, 0, 0), CV_FILLED);
		cvLine (e->image, point, cvPoint(point.x+direction.x, point.y+direction.y), CV_RGB (255, 0, 0), 3, CV_AA,0);

		// ターゲットとなっているコースの情報を画像に表示
		CourseIterator *iterator = ( e->course->getIterator(checkPointIndex) );
		CvPoint cp1 = *(iterator->getNextPoint()); 
		CvPoint cp2 = *(iterator->getNextPoint());
		cvCircle (e->image, cp1, 15, CV_RGB (0, 255, 0), CV_FILLED);
		cvLine (e->image, cp1, cp2, CV_RGB (0, 255, 0),10,CV_AA,0);
	}
	// 画像を表示
	//cvShowImage(windowNameRace, e->image);
	//this->cameraImage = e->image;
	this->avr4wdFrameListener->cameraImage = e->image;
	this->avr4wdFrameListener->setMachinePosition(e->currentPositionArray->at(0).x,e->currentPositionArray->at(0).y);
	this->avr4wdFrameListener->setMachineDirection(e->currentDirectionArray->at(0).x,e->currentDirectionArray->at(0).y);
	this->avr4wdFrameListener->setCameraPosition(e->currentPositionArray->at(0).x,e->currentPositionArray->at(0).y);
	this->avr4wdFrameListener->setCameraDirection(e->currentDirectionArray->at(0).x,e->currentDirectionArray->at(0).y);
	root->renderOneFrame();

}



/**
 * @bried    : IplImageにコースを書き込む関数
 * @pram[in] : IplImage, Course 型のオブジェクト
 * @return   : なし
 */
void Avr4wdMainWindow::drawCourse(IplImage *image, Course *course){
	CourseIterator *iterator = course->getIterator(0);
	CvPoint *prevPoint;
	
	if( iterator->hasNext() ) prevPoint = iterator->getNextPoint();
	cvCircle (image, *prevPoint, 3, CV_RGB (0, 0, 255), CV_FILLED);
	while( iterator->hasNext() ){
		CvPoint *point = (iterator->getNextPoint());
		cvCircle (image, *point, 3, CV_RGB (0, 0, 255), CV_FILLED);
		cvLine (image, *prevPoint, *point, CV_RGB (0, 0, 255));
		prevPoint = point;
	}

	int length = course->getCourseLength();
	if( length > 2 ){
		cvLine (image, *(course->getCoursePoint(length-1)), *(course->getCoursePoint(0)), CV_RGB (0, 0, 255));
	}
	
}

