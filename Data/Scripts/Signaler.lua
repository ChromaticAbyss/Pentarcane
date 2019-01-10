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
		--Message all objects named in the signalers string parameters
		count = 0
		name = GetScriptParameterString(0)
		while name ~= nil do
			target_id = GetIdFromUniqueName(name)
			if target_id ~= nil then 
				SendMessage(target_id,0,"Trigger")
			end 
			count = count + 1
			name = GetScriptParameterString(count)
		end 
		ScreenShake(0.05)
		PlaySound("../Data/Sounds/positive_trigger.ogg")
		Disable(my_id)
	end 
end 
