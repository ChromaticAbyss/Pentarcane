script_name = "Patrol_Transporter.lua"
my_id = -1


--Parameter usage:
--Strings are objects cycled though
cycle_positions = {}
cycle_length = 0
next_in_cycle = 1

speed = 0.10 --Parameter[0]
mode_reverse = 0 --Parameter[1], 0 cycle, 1 reverse, 2 stop at last point
reversing = 0

passenger_id = -1 

function setup()
	my_id = GetMyId() 
	
	passenger_name = GetScriptParameterString(0)
	passenger_id = GetIdFromUniqueName(passenger_name)
	
	count = 0
	name = GetScriptParameterString(1) --offset of 1 for passenger id
	while name ~= nil do
		target_id = GetIdFromUniqueName(name)
		if target_id ~= -1 then 
			px,py = GetPosition(target_id)
			cycle_positions[1 + #cycle_positions]={pos_x=px,pos_y=py}
		end 
		count = count + 1
		name = GetScriptParameterString(count + 1) --offset of 1 for passenger id
	end 
	cycle_length = count
	
	speed = GetScriptParameterFloat(0) or 0.10
	mode_reverse = GetScriptParameterFloat(1) or mode_reverse
		
end

function frame()
	
	--Handle Motion
	if cycle_length <= 1 then 
		return
	end
	
	
	distance = PatrolMovement(my_id,cycle_positions[next_in_cycle].pos_x,cycle_positions[next_in_cycle].pos_y,speed)
	
	if(distance<speed/2)then 
		reversing,next_in_cycle = grab_next_node(reversing,next_in_cycle,cycle_length,mode_reverse)
	end 

	
	if(passenger_id >= 0) then 
		mx,my = GetPosition(my_id)
		MoveTo(passenger_id,mx,my,0)
	end 
end 
