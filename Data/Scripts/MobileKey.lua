script_name = "MobileKey.lua"
my_id = -1


--Parameter usage:
--Strings are objects cycled though
cycle_positions = {}
cycle_length = 0
next_in_cycle = 1

speed = 0.10 --Parameter[0]
mode_reverse = 0 --Parameter[1], 0 cycle, 1 reverse, 2 stop at last point
reversing = 0

function setup()
	my_id = GetMyId() 
	
	count = 0
	name = GetScriptParameterString(0)
	while name ~= nil do
		target_id = GetIdFromUniqueName(name)
		if target_id ~= nil then 
			px,py = GetPosition(target_id)
			cycle_positions[1 + #cycle_positions]={pos_x=px,pos_y=py}
		end 
		count = count + 1
		name = GetScriptParameterString(count)
	end 
	cycle_length = count
	
	speed = GetScriptParameterFloat(0) or speed
	mode_reverse = GetScriptParameterFloat(1) or mode_reverse
		
end

function frame()
	possibly_player = CheckCollisionWithTag(my_id,"Player",0,0,0)
	if(possibly_player > -1)then 
		GrantLabyrinthKey()
		PlaySound("key.ogg")
		Disable(my_id)
	end 	
	--Handle Motion
	if cycle_length <= 1 then 
		return
	end
	
	
	distance = PatrolMovement(my_id,cycle_positions[next_in_cycle].pos_x,cycle_positions[next_in_cycle].pos_y,speed)
	

	
	if(distance<speed/2)then 
		reversing,next_in_cycle = grab_next_node(reversing,next_in_cycle,cycle_length,mode_reverse)
	end 

end 
