script_name = "ExplodingGroundTile_Triggered.lua"
my_id = -1

--Specialised
cycle_duration = 15 --Parameter[0]
child_object_off = -1
child_object_explosion = -1

where_in_cycle = -1

skip_frame = 1 --To avoid using our spawned id's before they are ready

triggered = 0

function setup()
	my_id = GetMyId() 
	my_x,my_y = GetPosition(my_id)
	
	child_object_off = Spawn("ExplodingGroundTile_Off",my_x,my_y,0)
	child_object_explosion = Spawn("ExplodingGroundTile_Explosion",my_x,my_y,0)	
	
	cycle_duration = GetScriptParameterFloat(0) or cycle_duration
	where_in_cycle = -1 --Now calculated in phases!

	
	Hide(my_id)
end

function frame()
	if(skip_frame == 1)then
		skip_frame = 0
		return 
	end
	if(skip_frame==0)then 
		skip_frame = -1
		--Late initialize
	end
	if(triggered == 1 and where_in_cycle == -1)then
		triggered = 0
		where_in_cycle = 0 --cycle_duration - 1
	end 
	if(where_in_cycle == -1)then
		return 
	end
	

	
	where_in_cycle = where_in_cycle + 1
	if(where_in_cycle > cycle_duration * 3)then
		where_in_cycle = -1
	end


	if(where_in_cycle <= 0)then
		retract_spike()
	end
	
	if(where_in_cycle == cycle_duration)then
		precharge_spike()
	end
	
	if(where_in_cycle == 2 * cycle_duration)then
		--Hide(my_id)
		--Unhide(child_object_explosion)
		pop_up_spike()
	end
	
end 


function pop_up_spike()
	SendMessage(child_object_explosion,0,"PopUp",0.7) --Message player object to slow itself by 30%
end 

function retract_spike()
		Unhide(child_object_off)
		--Hide(child_object_explosion)
		SendMessage(child_object_explosion,0,"Retract",0.7) --Message player object to slow itself by 30%
		
		Hide(my_id)
end 

function precharge_spike()
		Hide(child_object_off)
		Unhide(my_id)
end 



function message()
	m0 = GetMessageParameterString(0)
	
	if m0 == "Trigger" then 
		triggered = 1
	end --SpeedMult

end 

