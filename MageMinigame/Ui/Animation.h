#pragma once


#include <vector>
#include <string>

#include "../tinyxml2.h"

class Animation {
public:
	explicit Animation();
	explicit Animation(tinyxml2::XMLElement * xml_root);

	bool Running() const { return (frame < max_frame); };
	bool Blocking() const { return (frame < max_frame && blocking); };

	virtual bool Progress() = 0;

	
protected:
	void InitializeFromXml(tinyxml2::XMLElement * xml_root);//Not virtual on purpose!

	void PlaySound();

	bool DefaultProgress();
	int Frame() const { return frame; };
	int MaxFrame() const { return max_frame; };
private:
	int frame;
	int max_frame;
	bool blocking;
	std::string sound;
};