#pragma once

#include <vector>
#include <string>

struct InterObjectMessage {

	int recipient;

	std::vector<std::string> content;

};
