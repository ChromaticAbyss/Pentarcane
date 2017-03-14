#pragma once

#include <vector>
#include <string>
//Data carrier for the returning of data from labrinths/minigames

struct LabyrinthResult {
public:
	enum state_keys{running = 0, success = 1, failure = 2};

	LabyrinthResult();
	LabyrinthResult(state_keys,int,int,std::vector<std::string>);

	state_keys state; //0 running, 1 finished(sucess), 2finished(failure)
	int monster_id; //What monster does this belong to?
	int goals;
	std::vector<std::string> bonus_effects;//How many special effects were triggered
	bool fast;//Labyrinth was finished fast (what fast means depends on the labyrinth in question)
};