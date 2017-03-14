#include "Monster.h"

#include "MonsterDefinition.h"

#include "Log.h"

using namespace std;


Monster::Monster()
	:definition(nullptr),stats(),effect_block_attack(false)
	,internal_name("[Definitionless monster]"),model(),transform(),image()
{
};



Monster::Monster(Transform2D transf,const MonsterDefinition* definition_) 
	:definition(definition_),stats(definition->stats),effect_block_attack(false)
	,internal_name(definition->GiveName()),my_labyrinth(definition->RandomLabyrinth()),model(definition->model_def),transform(transf)
	,image()
{		

	auto sc_y = definition->image_transform.ScaleYAbs();

	//TODO: Positions seem to not have an effect, check that!

	image = Instance2D(definition->image_name,transf);

	//Since MonsterDefinition vektor does not change during the game, the pointer each instance takes can never become invalid!
};

std::vector<MonsterCast> Monster::DealDamage(int damage, bool ignore_armor) {
	stats = stats.DealDamage(damage, ignore_armor);
	if (stats.Cast_Chance() >= rand() % 100) {
		return definition->GiveCastWhenDamageTaken();
	}
	else {
		return vector<MonsterCast>();
	}

};


void Monster::Render(OpenGLContainer* open_gl) const{
	//glm::mat4 new_matrix = transform.MakeModelMatrix();
	//model.Render(new_matrix, open_gl);
}
void Monster::Render2D(OpenGLContainer * open_gl) const
{
	image.Render(transform.MakeModelMatrix(open_gl) * definition->image_transform.MakeModelMatrix(open_gl) ,open_gl);
}


std::string Monster::DisplayName() const
{
	return definition->GiveDisplayName();
}

void Monster::PassTurn()
{
	effect_block_attack = false;

}

vector<MonsterCast> Monster::MonsterAction() const {
	vector<MonsterCast> return_value;
	if (effect_block_attack == false) {
		if (definition->HasNoCasts() || stats.Cast_Chance() <= rand() % 100) {
			//Default attack
			return_value.push_back(MonsterCast(stats.Attack(), definition->DefaultAttackAnimation()));

		}
		else {
			Log("Debug", "Using Cast!");

			return_value = definition->GiveListOfCasts(); //Cast one from the list

		}
	}
	return return_value;
};