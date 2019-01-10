
function grab_next_node(reversing,next_in_cycle,cycle_length,mode_reverse)
	if(reversing==0)then 
		next_in_cycle = next_in_cycle + 1 	
	end 
	if(reversing==1)then 
		next_in_cycle = next_in_cycle - 1 
	end 
	
	
	if(next_in_cycle > cycle_length)then
		if(mode_reverse==1)then
			return 1,next_in_cycle - 1 --reversing,next_in_cycle
		end
		if(mode_reverse==0)then --No reverse mode: 
			return 0,1 --reversing,next_in_cycle
		end
		if(mode_reverse==2)then 
			return 0,cycle_length --reversing,next_in_cycle
		end 
	end
	
	if(next_in_cycle < 1)then
		return 0,2 --reversing,next_in_cycle
	end
	
	return reversing,next_in_cycle --reversing,next_in_cycle
end 

function PatrolMovement(my_id,tx,ty,speed)
	x,y = GetPosition(my_id)
	--tx = cycle_positions[next_in_cycle].pos_x
	--ty = cycle_positions[next_in_cycle].pos_y
	
	dx = tx - x
	dy = ty - y
	if(dx==0 and dy==0)then
		return 0
	end
	
	length = Sqrt(dx * dx + dy * dy)
	
	--print("actual distance moved: "..Sqrt((dx*speed/length)*(dx*speed/length) + (dy*speed/length)*(dy*speed/length)))
	--THis equals the "speed" with only tiniest rounding errors. Stays constant
	Move(my_id,dx*speed/length,dy*speed/length,0)
	
	dx2 = tx - (x + dx*speed/length)
	dy2 = ty - (y + dy*speed/length)
	if(dx==0 and dy==0)then
		return 0
	end
	length2 = Sqrt(dx2 * dx2 + dy2 * dy2)
	
	
	return length2
end 

