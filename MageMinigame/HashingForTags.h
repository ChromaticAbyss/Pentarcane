#pragma once

#include <string>
#include <vector>

namespace HashingForTags {


	class Hash_Thing {
		public:
			int Get(std::string);
		private:
			std::vector<std::string> names;
	};

	extern Hash_Thing tags;

}