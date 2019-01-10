#pragma once

#include <string>
#include "Animation.h"
#include "UiAnimation.h"
//#include "FlipbookAnimation.h"
#include "AnimationTickGiver.h"
#include "PolymorphicAnimation.h"
#include "CompositeAnimation.h"

class OpenGLContainer;

class AnimationManager : public PolymorphicAnimation
{
public:
	AnimationManager(AnimationTickGiver* tick_giver);

	bool blocking() const {
		return block_ticks > 0;
	}

	void tick() override {
		--block_ticks;
	}

	void AddAnimation(std::string file, const Transform2D& transform);

	void Render(OpenGLContainer*) const;

private:
	AnimationTickGiver* tick_giver;
	std::vector<std::unique_ptr<CompositeAnimation>> animations;

	int block_ticks;
};

