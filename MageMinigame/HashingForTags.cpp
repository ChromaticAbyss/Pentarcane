#include "HashingForTags.h"
#include <iostream>

using namespace std;

namespace HashingForTags {
	Hash_Thing tags;


	int Hash_Thing::Get(string n) {
		for (unsigned int i=0;i<names.size();i++){
			if (names[i] == n) {
				//cout << "found" << i << endl;
				return i;
			}
		}
		names.push_back(n);
		return names.size() - 1;
	}

};