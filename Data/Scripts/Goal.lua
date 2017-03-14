script_name = "Goal.lua"
my_id = -1

my_x = 0 --Fixed position we can buffer
my_y = 0
my_z = 0

spell_type = -1

function setup()
	my_id = GetMyId() 
	my_x,my_y,my_z = GetPosition(my_id)
	spell_type = GetSpellType()
end

function frame()
	--Disable player and grant one goal point
	possibly_player = CheckCollisionWithTag(my_id,"Player",0,0,0)
	if(possibly_player > -1)then 
		if(spell_type ~= 4)then --All spells other than DrainLife are done once they reach the goal
			GrantLabyrinthGoal()
		else
			Spawn("Player_Lifesteal_B",my_x,my_y,my_z,"Player_OnReturn") --Spawn the fully soaked return object for DrainLife
			Disable(my_id)
		end 
		PlaySound("Goal.wav")
		Disable(possibly_player)
		
	end 
end 
