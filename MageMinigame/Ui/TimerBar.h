#pragma once


#include "UiElement.h"

#include <string>
#include <vector>

class OpenGLContainer;


class TimerBar{
public:
	TimerBar();
	TimerBar(int, int, std::string);

	TimerBar(const TimerBar &) = delete;

	void Render(OpenGLContainer * open_gl) const;

	void Progress();
	bool Done() const { return (frames_left < 1); };
	int Id() const { return id; };

private:
	int id;
	int frames_left;
	int frames_total;

	std::string ui_name;
	std::unique_ptr<UiElement> ui;
};
