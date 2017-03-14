script_name = "Patrol_GearGuardian.lua"
my_id = -1

--Parameter usage:
--Strings are objects cycled though
cycle_positions = {}
cycle_length = 0
next_in_cycle = 1

speed = 0.10 --Parameter[0]
mode_reverse = 1 --Parameter[1]
reversing = 0
--Shares functionality with the default patrol object, TODO: how to implement this without code duplication?

function setup()
	my_id = GetMyId() 
	
	count = 0
	name = GetScriptParameterString(0)
	while name ~= nil do
		target_id = GetIdFromUniqueName(name)
		if target_id ~= -1 then 
			px,py = GetPosition(target_id)
			cycle_positions[1 + #cycle_positions]={pos_x=px,pos_y=py}
		end 
		count = count + 1
		name = GetScriptParameterString(count)
	end 
	cycle_length = count
	
	speed = GetScriptParameterFloat(0) or 0.10
	mode_reverse = GetScriptParameterFloat(1) or 1
		
end

function frame()	
	--Kill player on contact
	possibly_player = CheckCollisionWithTag(my_id,"Player",0,0,0)
	if(possibly_player > -1)then 
		--Disable(possibly_player)
		SendMessage(possibly_player,0,"Die")
	end 
	--Handle Motion
	if cycle_length <= 1 then 
		return
	end
	
	-------------------------------------Don't Move if the player isn't moving
	player_dx,player_dy = GetInputs()
	if(player_dx ==0 and player_dy==0)then
		return
	end
	-------------------------------------
	
	
	distance = PatrolMovement(my_id,cycle_positions[next_in_cycle].pos_x,cycle_positions[next_in_cycle].pos_y,speed)
	

	if(distance<0.2)then 
		reversing,next_in_cycle = grab_next_node(reversing,next_in_cycle,cycle_length,mode_reverse)
	end 

end 

