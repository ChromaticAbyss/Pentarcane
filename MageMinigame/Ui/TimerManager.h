#pragma once

#include "TimerBar.h"

#include <vector>
#include <string>
#include <list>

class TimerManager {
public:
	TimerManager();

	void AddBar(int,int,std::string);

	void Progress();

	void Render(OpenGLContainer * open_gl) const;

private:

	std::list<TimerBar> bars;


};
