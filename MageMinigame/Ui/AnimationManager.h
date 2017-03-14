#pragma once

#include "Animation.h"
#include "UiAnimation.h"
#include "FlipbookAnimation.h"

class OpenGLContainer;

class AnimationManager
{
public:
	AnimationManager();

	bool Blocking() const;

	void Progress();

	void AddAnimation( UiAnimation&& n);
	void AddAnimation( FlipbookAnimation&& n);

	void Render(OpenGLContainer*) const;

private:
	std::vector<UiAnimation> ui_animations;
	std::vector<FlipbookAnimation> flipbook_animations;
};

