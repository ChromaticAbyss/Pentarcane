script_name = "Shroud_Base.lua"
my_id = -1

--Specialised

child_object = -1

skip_frame = 1 --To avoid using our spawned id's before they are ready

triggered = 0

function setup()
	my_id = GetMyId() 
	my_x,my_y = GetPosition(my_id)
	
	child_object = Spawn("Shroud_Shroud",my_x,my_y,0)
	
end

function frame()
	if(skip_frame==1)then
		skip_frame = 0
		return 
	end
	
	
	if(triggered == 1)then 
		SendMessage(child_object,0,"Dissappear")
		PlaySound("shroud_remove.ogg")
		triggered = 0
	end 


	
end 



function message()
	m0 = GetMessageParameterString(0)
	
	if m0 == "Trigger" then 
		triggered = 1
	end --SpeedMult

end 
