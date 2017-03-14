#pragma once

#include <string>
#include <vector>

class ScriptReturn {
public:

	//small utility
	void SelfIs(int);

	//No encapsulation of this data as the point of this class is transfering it
	std::string type;
	std::vector<int> target_parts;
	std::vector<float> float_values;
	std::vector<std::string> string_values;

private:

};