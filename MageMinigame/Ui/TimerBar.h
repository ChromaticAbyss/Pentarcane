#pragma once


#include "PolymorphicUiElement.h"
#include "Spell.h"
#include "BarUiElement.h"

#include <string>
#include <vector>
#include <memory>

class OpenGLContainer;

class TimerBar{
public:
	TimerBar(OpenGLContainer * open_gl);
	TimerBar(int frames, Spell mode, OpenGLContainer * open_gl);

	TimerBar(const TimerBar &) = delete;
	TimerBar(TimerBar&& other) = default;
	TimerBar& operator=(TimerBar&& other) = default;

	void render(OpenGLContainer * open_gl) const;

	void Progress();
	bool Done() const { return (frames_left < 1); };

	void nullify();

private:
	std::unique_ptr<PolymorphicUiElement> stun_background() const;
	std::unique_ptr<PolymorphicUiElement> distraction_background() const ;
	void makeBar(const std::string& file);

	Spell mode;
	int frames_left;
	int frames_total;
	OpenGLContainer * open_gl;

	std::string ui_name;
	std::unique_ptr<PolymorphicUiElement> ui;
	std::unique_ptr<BarUiElement> bar;
};
