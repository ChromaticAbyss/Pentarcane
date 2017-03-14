script_name = "ExplodingGroundTile.lua"
my_id = -1

--Specialised
cycle_duration = 25 --Parameter[0]
offset = 0 --Parameter[1]
child_object_off = -1
child_object_explosion = -1

where_in_cycle = -1

skip_frame = 1 --To avoid using our spawned id's before they are ready


function setup()
	my_id = GetMyId() 
	my_x,my_y = GetPosition(my_id)
	
	child_object_off = Spawn("ExplodingGroundTile_Off",my_x,my_y,0)
	child_object_explosion = Spawn("ExplodingGroundTile_Explosion",my_x,my_y,0)	
	
	cycle_duration = GetScriptParameterFloat(0) or cycle_duration
	offset = GetScriptParameterFloat(1) or offset
	where_in_cycle = offset * cycle_duration --Now calculated in phases!

	
	Hide(my_id)
end

function frame()
	if(skip_frame==1)then
		skip_frame = 0
		return 
	end
	
	if(skip_frame==0)then 
		skip_frame = -1
		--Late initialize
		if(offset>=1 and offset<2) then
			precharge_spike()
		end
		if(offset>=2 and offset<3) then 
			pop_up_spike()
		end
			
	end
	
	
	where_in_cycle = where_in_cycle + 1
	if(where_in_cycle > cycle_duration * 3)then
		where_in_cycle = 0
	end


	if(where_in_cycle == 0)then
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
