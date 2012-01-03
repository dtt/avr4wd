#include "RaceTrackingObject.h"
#include "MachineController.h"
#include "Avr4wdMainWindow.h"

#ifndef __EDIT_MODE__

//スタートアップクラス

int main(int argc, char* argv[])
{
	Race                 *race = new RaceTrackingObject("log.xml");

	MachineObserver        *mc = new MachineController("COM5", serial::BaudRate::B38400);
	RaceObserver       *window = new Avr4wdMainWindow();
	
	race->attachRaceObserver(window);
	race->attachMachineObserver(mc);
	race->startRaceTracking();
	race->detachRaceObserver(window);
	race->detachMachineObserver(mc);

	delete window;
	delete mc;
	delete race;

	return 0;
}


#endif
