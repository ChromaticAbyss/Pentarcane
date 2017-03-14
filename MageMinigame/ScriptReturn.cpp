#include "ScriptReturn.h"

using namespace std;

void ScriptReturn::SelfIs(int self_id) {
	for (vector<int>::iterator it = target_parts.begin(); it != target_parts.end(); ++it) {
		if (*it <= -1) {
			*it = self_id;
		}
	}


};