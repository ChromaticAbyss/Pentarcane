script_name = "Patrol_ThroughPoints.lua"
my_id = -1
step = 0

--Parameter usage:
--Strings are objects cycled though
cycle_positions = {}
cycle_length = 0
next_in_cycle = 1

speed = 0.10 --Parameter[0]
mode_reverse = 0 --Parameter[1], 0 cycle, 1 reverse, 2 stop at last point
reversing = 0

distance_traveled = 0


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
	cycle_length = #cycle_positions --TODO: This should be #cycle_positions, shouldn't it
	
	speed = GetScriptParameterFloat(0) or speed
	mode_reverse = GetScriptParameterFloat(1) or mode_reverse
end

function frame()
	step = step + 1
	EnemyPlayerCollision("Patrol")
	 
	--Handle Motion
	if cycle_length <= 1 then 
		return
	end
	
	--pre_x,pre_y = GetPosition(my_id)
	
		distance = PatrolMovement(my_id,cycle_positions[next_in_cycle].pos_x,cycle_positions[next_in_cycle].pos_y,speed)
	
	--post_x,post_y = GetPosition(my_id)
	--d_x = post_x - pre_x
	--d_y = post_y - pre_y
	--actual_distance = Sqrt(d_x*d_x + d_y * d_y )
	
	--distance_traveled = distance_traveled + actual_distance --This is different, causing the divergence
	
	if(distance < speed*(0.5))then 
		--print("Turned after moving "..distance_traveled)
		--distance_traveled = 0
		reversing,next_in_cycle = grab_next_node(reversing,next_in_cycle,cycle_length,mode_reverse)
	end 

	animate()
end 

function animate()
	if(step % 7 == 0)then 
		my_x,my_y = GetPosition(my_id)		
		StartAnimation("EnemyPatrolTrail",my_x,my_y,0.1)
	end 
end 