#include "Avr4wdFrameListener.h"

Avr4wdFrameListener::Avr4wdFrameListener(Ogre::SceneManager* sceneMgr, Ogre::RenderWindow* window){
	this->sceneMgr = sceneMgr;
	this->window = window;
	this->flagNode = NULL;
	
	this->sceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_MODULATIVE);
	this->sceneMgr->setShadowFarDistance(1000);

	Ogre::SceneNode* node = this->sceneMgr->getRootSceneNode()->createChildSceneNode();
	this->cameraNode = node->createChildSceneNode();

	this->cam = this->sceneMgr->createCamera("MainCamera");
	Ogre::Viewport* port = window->addViewport(cam);
	this->cam->setPosition(0,0,0);
	this->cam->lookAt(0,0,0);
	this->cam->setNearClipDistance(5);
	this->cam->setAspectRatio(Ogre::Real(port->getActualWidth())/Ogre::Real(port->getActualHeight()));
	this->cameraNode->attachObject(this->cam);
	this->cameraNode->translate(0,100,0);

	port->setBackgroundColour(Ogre::ColourValue(0,0,0));
	
	Ogre::Light* directionalLight = this->sceneMgr->createLight("directionalLight");
	directionalLight->setType(Ogre::Light::LT_POINT);
	directionalLight->setDiffuseColour(Ogre::ColourValue(1,1,1));
	directionalLight->setSpecularColour(Ogre::ColourValue(1,1,1));
	directionalLight->setPosition(5000,2000,5000);
	directionalLight->setCastShadows(true);

	this->sceneMgr->setAmbientLight(Ogre::ColourValue(1,1,1));

	frameWidth = 480;
	frameHeight = 640;
	
	cameraImageBGRA = cvCreateImage(cvSize(frameWidth,frameHeight),IPL_DEPTH_8U,4);

	OIS::ParamList pl;
	std::ostringstream windowHndStr;
	size_t windowHnd = 0;
	this->window->getCustomAttribute("WINDOW",&windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"),windowHndStr.str()));
	this->manager = OIS::InputManager::createInputSystem(pl);
	this->keyboard = static_cast<OIS::Keyboard*> (this->manager->createInputObject(OIS::OISKeyboard,true));
	
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create("PlaneMat",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	texture = Ogre::TextureManager::getSingleton().createManual(
		"DynamicTexture",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D,
		frameWidth,
		frameHeight,
		0,
		Ogre::PF_BYTE_BGRA,
		Ogre::TU_DEFAULT
		);

	Ogre::HardwarePixelBufferSharedPtr pixelBuffer = texture->getBuffer();
	pixelBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL);
	const Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();
	Ogre::uint8* pDest = static_cast<Ogre::uint8*>(pixelBox.data);
	
	//memcpy(pDest,cameraImageBGRA->imageData,frameWidth*frameHeight*4);
	pixelBuffer->unlock();
	
	Ogre::TextureUnitState* tuisTexture = mat->getTechnique(0)->getPass(0)->createTextureUnitState("DynamicTexture");
	mat->setReceiveShadows(true);
	
	this->sceneMgr->setSkyDome(true,"Examples/CloudySky",5,5);
	
	Ogre::SceneNode* cameraPlaneNode = node->createChildSceneNode();
	Ogre::SceneNode* outerPlaneNode = node->createChildSceneNode();
	this->flagNode = node->createChildSceneNode();
	
	{
		Ogre::Plane p;
		p.d = 0;
		p.normal = Ogre::Vector3::UNIT_Y;
		Ogre::MeshManager::getSingleton().createPlane("FloorPlane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, p, 200000, 200000, 20, 20, true, 1, 9000, 9000, Ogre::Vector3::UNIT_Z);
		Ogre::Entity* ent = sceneMgr->createEntity("OuterPlane", "FloorPlane");
		ent->setMaterialName("Examples/GrassFloor");
		ent->setCastShadows(true);
		outerPlaneNode->attachObject(ent);		
		outerPlaneNode->translate(0,-1,0);
	}
	{
		Ogre::Plane plane;
		plane.d = 0;
		plane.normal = Ogre::Vector3::UNIT_Y;
		Ogre::MeshManager::getSingleton().createPlane("PlaneMesh",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,plane,frameWidth,frameHeight,1,1,true,1,1,1,Ogre::Vector3::UNIT_Z);

		Ogre::Entity* ent = this->sceneMgr->createEntity("PlaneEntity","PlaneMesh");
		ent->setMaterialName("PlaneMat");
		ent->setCastShadows(true);
		cameraPlaneNode->attachObject(ent);
	}
	Ogre::SceneNode* entNode = node->createChildSceneNode();

	{
		Ogre::Entity* ent = this->sceneMgr->createEntity("Pylon1","Pylon.mesh");
		ent->setCastShadows(true);
		Ogre::SceneNode* pylonNode = entNode->createChildSceneNode();
		pylonNode->attachObject(ent);
		pylonNode->translate(frameWidth/2,0,frameHeight/2,Ogre::Node::TS_PARENT);
		pylonNode->scale(25,25,25);
	}
	{
		Ogre::SceneNode* pylonNode = entNode->createChildSceneNode();
		Ogre::Entity* ent = this->sceneMgr->createEntity("Pylon2","Pylon.mesh");
		ent->setCastShadows(true);
		pylonNode->attachObject(ent);
		pylonNode->translate(-frameWidth/2,0,frameHeight/2,Ogre::Node::TS_PARENT);
		pylonNode->scale(25,25,25);
	}
	{
		Ogre::SceneNode* pylonNode = entNode->createChildSceneNode();
		Ogre::Entity* ent = this->sceneMgr->createEntity("Pylon3","Pylon.mesh");
		ent->setCastShadows(true);
		pylonNode->attachObject(ent);
		pylonNode->translate(frameWidth/2,0,-frameHeight/2,Ogre::Node::TS_PARENT);
		pylonNode->scale(25,25,25);
	}
	{
		Ogre::SceneNode* pylonNode = entNode->createChildSceneNode();
		Ogre::Entity* ent = this->sceneMgr->createEntity("Pylon4","Pylon.mesh");
		ent->setCastShadows(true);
		pylonNode->attachObject(ent);
		pylonNode->translate(-frameWidth/2,0,-frameHeight/2,Ogre::Node::TS_PARENT);
		pylonNode->scale(25,25,25);
	}
	{
		Ogre::SceneNode* pylonNode = entNode->createChildSceneNode();
		Ogre::Entity* ent = this->sceneMgr->createEntity("StartPosition","StartPosition.mesh");
		ent->setCastShadows(true);
		ent->getBoundingBox().getHalfSize().x;
		pylonNode->attachObject(ent);
		Ogre::Vector3 vec = ent->getBoundingBox().getHalfSize();
		pylonNode->translate(0,0,frameHeight/2-ent->getBoundingBox().getHalfSize().z*15,Ogre::Node::TS_PARENT);
		pylonNode->scale(15,15,15);
	}
	{
		Ogre::SceneNode* pylonNode = entNode->createChildSceneNode();
		Ogre::Entity* ent = this->sceneMgr->createEntity("Stand1","Stand.mesh");
		ent->setCastShadows(true);
		ent->getBoundingBox().getHalfSize().x;
		pylonNode->attachObject(ent);
		Ogre::Vector3 vec = ent->getBoundingBox().getHalfSize();
		pylonNode->translate(0,0,-frameHeight/2-ent->getBoundingBox().getHalfSize().z*30,Ogre::Node::TS_PARENT);
		pylonNode->scale(30,30,30);
	}
	for(int i = 0;i < 12;i++)
	{
		Ogre::SceneNode* pylonNode = entNode->createChildSceneNode();
		Ogre::Entity* ent = this->sceneMgr->createEntity("Human" + Ogre::StringConverter::toString(i),"HumanBlue.mesh");
		ent->setCastShadows(true);
		ent->getBoundingBox().getHalfSize().x;
		pylonNode->attachObject(ent);
		Ogre::Vector3 vec = ent->getBoundingBox().getHalfSize();
		pylonNode->translate(-70+(ent->getBoundingBox().getSize().z+0.1)*20*i,20,-frameHeight/2-80,Ogre::Node::TS_PARENT);
		pylonNode->scale(20,20,20);
	}
	for(int i = 0;i < 12;i++)
	{
		Ogre::SceneNode* humanNode = entNode->createChildSceneNode();
		Ogre::Entity* ent = this->sceneMgr->createEntity("Human" + Ogre::StringConverter::toString(i+12),"HumanBlue.mesh");
		ent->setCastShadows(true);
		ent->getBoundingBox().getHalfSize().x;
		humanNode->attachObject(ent);
		Ogre::Vector3 vec = ent->getBoundingBox().getHalfSize();
		humanNode->translate(-70+(ent->getBoundingBox().getSize().z+0.1)*20*i,10,-frameHeight/2-50,Ogre::Node::TS_PARENT);
		humanNode->scale(20,20,20);
	}
	{
		machineNode = entNode->createChildSceneNode();
		Ogre::Entity* ent = this->sceneMgr->createEntity("BuckBlader","BuckBlader.mesh");
		ent->setCastShadows(true);
		ent->getBoundingBox().getHalfSize().x;
		machineNode->attachObject(ent);
		Ogre::Vector3 vec = ent->getBoundingBox().getHalfSize();
		//machineNode->translate(-70+(ent->getBoundingBox().getSize().z+0.1)*20*i,10,-frameHeight/2-50,Ogre::Node::TS_PARENT);
		machineNode->scale(20,20,20);
	}
	/*
	{
		Ogre::SceneNode* flagNode = entNode->createChildSceneNode();
		Ogre::Entity* ent = this->sceneMgr->createEntity("FlagSample","Flag.mesh");
		ent->setCastShadows(true);
		ent->getBoundingBox().getHalfSize().x;
		flagNode->attachObject(ent);
		Ogre::Vector3 vec = ent->getBoundingBox().getHalfSize();
		flagNode->translate(0,30,0,Ogre::Node::TS_PARENT);
		flagNode->scale(300,300,300);
	}
	*/
}

Avr4wdFrameListener::~Avr4wdFrameListener(){
	
}

void Avr4wdFrameListener::setCameraPosition(int x, int y){
	this->cameraNode->setPosition(frameWidth/2 - x,20,frameHeight/2 - y);
}

void Avr4wdFrameListener::setCameraDirection(int x, int y){
	this->cameraNode->setDirection(Ogre::Vector3(-x,0,-y),Ogre::Node::TS_PARENT);
}

void Avr4wdFrameListener::setMachineDirection(int x, int y){
	this->machineNode->setDirection(Ogre::Vector3(-x,0,-y),Ogre::Node::TS_PARENT);
}

void Avr4wdFrameListener::setMachinePosition(int x,int y){
	//this->machineNode->setPosition(0,0,0);
	this->machineNode->setPosition(frameWidth/2 - x,0,frameHeight/2 - y);
	//this->machineNode->translate(frameWidth/2 - x,0,frameHeight/2 - y,Ogre::Node::TS_PARENT);
}

bool Avr4wdFrameListener::setCourseFlag(Course* course, int frameWidth,int frameHeight){
	bool status = true;
	CourseIterator *iterator = course->getIterator(0);
	int i = 0;
	if(this->flagNode != NULL){
		while( iterator->hasNext() ){
			CvPoint *point = (iterator->getNextPoint());
			{
				Ogre::SceneNode* node = this->flagNode->createChildSceneNode();
				Ogre::Entity* ent = this->sceneMgr->createEntity("Flag" + Ogre::StringConverter::toString(i++),"Flag.mesh");
				ent->setCastShadows(true);
				node->attachObject(ent);
				node->setScale(300,300,300);
				Ogre::Vector3 vec = ent->getBoundingBox().getHalfSize();
				node->translate(frameWidth/2 - point->x + vec.x * 300,0,frameHeight/2 - point->y + vec.y * 300,Ogre::Node::TS_PARENT);
			}
		}
	}else{
		status = false;
	}
	return status;
}

bool Avr4wdFrameListener::frameEnded(const Ogre::FrameEvent& evt){
	return true;
}

bool Avr4wdFrameListener::frameRenderingQueued(const Ogre::FrameEvent& evt){
	this->keyboard->capture();
	if (this->keyboard->isKeyDown(OIS::KC_A)) {
		if (this->keyboard->isKeyDown(OIS::KC_UP)) {
			this->cam->pitch(Ogre::Radian(3.141592 / 128));
		}
		if (this->keyboard->isKeyDown(OIS::KC_DOWN)) {
			this->cam->pitch(Ogre::Radian(-3.141592 / 128));
		}
		if (this->keyboard->isKeyDown(OIS::KC_LEFT)) {
			this->cam->yaw(Ogre::Radian(3.141592 / 128));
		}
		if (this->keyboard->isKeyDown(OIS::KC_RIGHT)) {
			this->cam->yaw(Ogre::Radian(-3.141592 / 128));
		}
	} else {

		if (this->keyboard->isKeyDown(OIS::KC_U)) {
			this->cam->move(Ogre::Vector3(0, 3, 0));
		}
		if (this->keyboard->isKeyDown(OIS::KC_D)) {
			this->cam->move(Ogre::Vector3(0, -3, 0));
		}
		if (this->keyboard->isKeyDown(OIS::KC_UP)) {
			this->cam->moveRelative(Ogre::Vector3(0, 0, -3));
		}
		if (this->keyboard->isKeyDown(OIS::KC_DOWN)) {
			this->cam->moveRelative(Ogre::Vector3(0, 0, 3));
		}
		if (this->keyboard->isKeyDown(OIS::KC_LEFT)) {
			this->cam->moveRelative(Ogre::Vector3(-3, 0, 0));
		}
		if (this->keyboard->isKeyDown(OIS::KC_RIGHT)) {
			this->cam->moveRelative(Ogre::Vector3(3, 0, 0));
		}
		if (this->keyboard->isKeyDown(OIS::KC_Q)) {
			delete this;
			exit(0);
		}

	}
	return true;
}

bool Avr4wdFrameListener::frameStarted(const Ogre::FrameEvent& evt){
	
	Ogre::HardwarePixelBufferSharedPtr pixelBuffer = texture->getBuffer();
	
	pixelBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
	
	const Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();
	Ogre::uint8* pDest = static_cast<Ogre::uint8*>(pixelBox.data);
	
	if(this->cameraImage != NULL){
		cvCvtColor(this->cameraImage,this->cameraImageBGRA,CV_BGR2BGRA);
	}
	
	memcpy(pDest,this->cameraImageBGRA->imageData,frameWidth*frameHeight*4);
	
	pixelBuffer->unlock();
	
	return true;
}


