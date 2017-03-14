script_name = "ShooterBullet.lua"
my_id = -1

target_available = false
target_x = 0
target_y = 0
speed = 0.25

fracture_ids = {}

function setup()
	my_id = GetMyId() 
		
end

function frame()
	if(target_available == false)then 
		return 
	end 

	--Kill player on contact
	possibly_player = CheckCollisionWithTag(my_id,"Player",0,0,0)
	if(possibly_player > -1)then 
		--Disable(possibly_player)
		SendMessage(possibly_player,0,"Die")
	end 	
	
	--Move toward target
	distance = PatrolMovement(my_id,target_x,target_y,speed)
	

	
	if(distance<speed/2)then 
		--reversing,next_in_cycle = grab_next_node(reversing,next_in_cycle,cycle_length,mode_reverse)
		if(#fracture_ids > 0)then 
			my_x,my_y = GetPosition(my_id)
			for i=1,#fracture_ids,1 do
				new_proj = Spawn("Shooter_Bullet",my_x,my_y,0)	
				SendMessage(new_proj,0,"YourTarget",fracture_ids[i])
			end 
		end 
		
		Disable(my_id)
	end 	

end 




function message()
	m0 = GetMessageParameterString(0)
	
	if m0 == "SpeedMult" then 
		speed = speed * ( GetMessageParameterString(1))
	end --SpeedMult

	
	
	if m0 == "YourTarget" then
		m1 = GetMessageParameterString(1)
		if(m1 ~= nil)then 
			target_x,target_y = GetPosition(m1)
			target_available = true
		end
	end --SpeedMult
	
	
	
	if m0 == "YourFractureTarget" then
		m1 = GetMessageParameterString(1)
		if(m1 ~= nil)then 
			fracture_ids[#fracture_ids + 1] = m1
		end
	end --SpeedMult
		
	
end 
