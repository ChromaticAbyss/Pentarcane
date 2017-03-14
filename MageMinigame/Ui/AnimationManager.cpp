#include "AnimationManager.h"

#include "../OpenGLContainer.h"

using namespace std;

AnimationManager::AnimationManager()
	:ui_animations(),flipbook_animations()
{

}

bool AnimationManager::Blocking() const
{
	for (auto it = ui_animations.cbegin(); it != ui_animations.cend(); ++it) {
		if (it->Blocking()) {
			return true;
		}
	}
	for (auto it = flipbook_animations.cbegin(); it != flipbook_animations.cend(); ++it) {

		if (it->Blocking()) {
			return true;
		}
	}

	return false;
}

void AnimationManager::Progress()
{
	for (auto it = ui_animations.begin(); it != ui_animations.end(); ++it) {
		it->Progress();
	}
	for (auto it = flipbook_animations.begin(); it != flipbook_animations.end(); ++it) {
		it->Progress();
	}
}

void AnimationManager::AddAnimation( UiAnimation&& n)
{
	ui_animations.push_back(std::move(n));
}

void AnimationManager::AddAnimation( FlipbookAnimation&& n)
{

	flipbook_animations.push_back(std::move(n));
}

void AnimationManager::Render(OpenGLContainer * open_gl) const
{
	for (auto it = ui_animations.cbegin(); it != ui_animations.cend(); ++it) {
		it->Render(open_gl);
	}

	for (auto it = flipbook_animations.cbegin(); it != flipbook_animations.cend(); ++it) {
		it->Render(open_gl);
	}
}
