#include "AnimationManager.h"
#include "tinyxml2.h"

#include "OpenGLContainer.h"
#include "Log.h"
#include "XMLUtil.h"
#include "Behaviours\behaviourbase.h"
#include "BehaviourContainer.h"
#include "Behaviours\ParticleBehaviourDispatcher.h"

#include "IrrklangSingleton.h"

using namespace std;

AnimationManager::AnimationManager(AnimationTickGiver* tick_giver_in)
	:PolymorphicAnimation(tick_giver_in), animations(),tick_giver(tick_giver_in), block_ticks(0)
{

}


void AnimationManager::AddAnimation(std::string file, const Transform2D& transform)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile(file.c_str());

	if (doc.ErrorID() != 0) {
		int i = 7;
		return;
	}

	tinyxml2::XMLElement* root = doc.FirstChildElement("Animation");
	if (root) {

		int frames = LoadSingleValue_Required<int>(root, "Frames");
		int block = LoadSingleValue(root, "BlockFrames", 0);
		if (block > block_ticks) {
			block_ticks = block;
		}

		//TODO: Sounds
		std::string sound = LoadSingleString(root, "Sound", "");
		if (sound != "") {
			IrrklangSingleton::get()->play2D(sound.c_str(), false);
		}

		auto behaCo = LoadVectorOf<BehaviourContainer>(root, "Behaviour");

		auto temp = std::make_unique<CompositeAnimation>(tick_giver,
			transform,
			frames,
			std::move(behaCo)
		);

		animations.push_back(
			std::move(temp)
		);

	} else {
		Log("Error", "Animations need <Animation> as root file: "+file);
	}

}

void AnimationManager::Render(OpenGLContainer * open_gl) const
{
	static Transform2D anim_transform(TransformPosition(0, 0), TransformScale(1.0f, 1.0f, PercentOfScreen::ax_x));
	for (const auto& anim : animations) {
		anim->render(anim_transform.MakeModelMatrix(open_gl), open_gl);
	}
}
