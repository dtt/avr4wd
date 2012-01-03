#pragma once

#include <Ogre.h>
#include <OIS.h>
#include "OpenCV.h"
#include "Course.h"

class Avr4wdFrameListener : public Ogre::FrameListener {
public:
	Avr4wdFrameListener(Ogre::SceneManager* sceneMgr, Ogre::RenderWindow* window);
	~Avr4wdFrameListener(void);

	bool frameStarted(const Ogre::FrameEvent &evt);
	bool frameEnded(const Ogre::FrameEvent &evt);
	bool frameRenderingQueued(const Ogre::FrameEvent &evt);

	bool setCourseFlag(Course* course, int frameWidth,int frameHeight);

	void setMachinePosition(int x, int y);
	void setMachineDirection(int x, int y);
	void setCameraDirection(int x, int y);
	void setCameraPosition(int x, int y);

	IplImage* cameraImage;

private:
	Ogre::SceneManager* sceneMgr;
	OIS::Keyboard* keyboard;
	OIS::Mouse* mouse;
	OIS::InputManager* manager;

	Ogre::RenderWindow* window;
	Ogre::Camera* cam;
	Ogre::TexturePtr texture;
	size_t val;

	CameraControl camCon;
	int frameWidth,frameHeight;
	
	
	IplImage* cameraImageBGRA;

	Ogre::SceneNode* cameraNode;
	Ogre::SceneNode* flagNode;
	Ogre::SceneNode* machineNode;
};