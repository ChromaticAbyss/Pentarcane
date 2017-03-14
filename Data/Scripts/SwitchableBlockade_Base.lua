script_name = "ExplodingGroundTile.lua"
my_id = -1

--Specialised

child_object_rotor = -1
state = 0

skip_frame = 1 --To avoid using our spawned id's before they are ready

triggered = 0

function setup()
	my_id = GetMyId() 
	my_x,my_y = GetPosition(my_id)
	
	child_object_rotor = Spawn("Switchable_Blockade_Rotor",my_x,my_y,0)	
	
	start_as = GetScriptParameterFloat(0) or 1
	if(start_as > 0.5)then 
		state = 1
		SendMessage(child_object_rotor,0,"PopUp",0.7)
	end
	
	
end

function frame()
	if(skip_frame==1)then
		skip_frame = 0
		return 
	end
	
	
	if(triggered == 1)then 
		triggered = 0
		if(state == 0)then 
			SendMessage(child_object_rotor,0,"PopUp",0.7)
			
			state = 1
			return 
		end 
		if(state == 1)then 
			SendMessage(child_object_rotor,0,"Retract",0.7)
			
			state = 0
			return 
		end 
	end 


	
end 



function message()
	m0 = GetMessageParameterString(0)
	
	if m0 == "Trigger" then 
		triggered = 1
	end --SpeedMult

end 
