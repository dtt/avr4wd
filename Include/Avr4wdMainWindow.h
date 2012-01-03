#pragma once

#include <Ogre.h>
#include "OpenCV.h"
#include "Avr4wdFrameListener.h"
#include "RaceObserver.h"
#include "Event.h"

class Avr4wdMainWindow :  public RaceObserver{

public:
	Avr4wdMainWindow(void);
	~Avr4wdMainWindow(void);
	Avr4wdFrameListener* avr4wdFrameListener;
	void updateEventHandler(UpdateEvent *e);
	void drawCourse(IplImage *image, Course *course);

private:
	Ogre::SceneManager* sceneMgr;
	Ogre::Camera* cam;
	Ogre::SceneNode* node;
	Ogre::Entity* ent;
	Ogre::Root* root;
	bool hasFlag;
};