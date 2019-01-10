/*#include "Animation.h"

#include "../Log.h"


#include "../IrrklangSingleton.h"


using namespace std;

Animation::Animation() //test-dummy animation
	:frame(0),max_frame(-1),blocking(true)
{

}

Animation::Animation(tinyxml2::XMLElement * xml_root) //test-dummy animation
	: frame(0), max_frame(20), blocking(true), sound_volume(1.0f)
{
	
}





bool Animation::Progress()
{
	return DefaultProgress();
}

void Animation::InitializeFromXml(tinyxml2::XMLElement * xml_root)
{

	{ 
		XMLElement * xml_child = xml_root->FirstChildElement("Frames");
		if (xml_child != nullptr) {
			max_frame = stoi(xml_child->GetText());
			frame = 0;
		}
	}

	{
		XMLElement * xml_child = xml_root->FirstChildElement("Blocking");
		if (xml_child != nullptr) {
			int a = stoi(xml_child->GetText());
			if (a == 0) { blocking = false; }
			if (a == 1) { blocking = true; }
		}
	}

	{
		XMLElement * xml_child = xml_root->FirstChildElement("Sound");
		if (xml_child != nullptr) {
			sound = xml_child->GetText();
		}
	}

	{
		XMLElement * xml_child = xml_root->FirstChildElement("SoundVolume");
		if (xml_child != nullptr) {
			sound_volume = stof(xml_child->GetText());
		}
	}
}

void Animation::PlaySound()
{
	irrklang::ISound* a = IrrklangSingleton().Get()->play2D(("../Data/Sounds/" + sound).c_str(), false, false, true);
	if (a) {
		a->setVolume(sound_volume);
	}
}

bool Animation::DefaultProgress()
{
	frame++;
	if (frame == 1) {
		PlaySound();
	}

	if (frame >= max_frame) {
		blocking = false;
		return true; //true = animation finished
	}

	return false; //false = animation still going
}
*/