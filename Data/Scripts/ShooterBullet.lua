script_name = "ShooterBullet.lua"
my_id = -1

target_available = false
target_x = 0
target_y = 0
speed = 0.25

fracture_ids = {}

explode_at_end = 0 --0 no, 1 yes, 2 exploding right now
explode_frame = 0
explode_frame_max = 8

function setup()
	my_id = GetMyId() 
	Hide(my_id,1)	
end

function frame()
	if(explode_at_end == 2)then 
		explodeBehaviour() --handle sbehaviour while exploding
		return 
	end 
	
	
	EnemyPlayerCollision("Bullet")

	

	
	
	if(target_available == false)then 
		return 
	end 
	
	--Move toward target
	distance = PatrolMovement(my_id,target_x,target_y,speed)
	

	
	if(distance<speed/2)then 
		if(#fracture_ids > 0)then 
			my_x,my_y = GetPosition(my_id)
			for i=1,#fracture_ids,1 do
				new_proj = Spawn("Shooter_Bullet",my_x,my_y,0)	
				SendMessage(new_proj,0,"YourTarget",fracture_ids[i])
				if(explode_at_end~=0)then 
					SendMessage(new_proj,0,"ExplodeAtTheEnd")
				end 
			end 
			
			Disable(my_id)--If we fracture we don't explode. Our children do however
		end 
		
		if(explode_at_end==0)then 
			Disable(my_id)
			return
		else 
			ScreenShake(0.025)
			explode_at_end = 2
			Hide(my_id,0)	
			Unhide(my_id,1)	
		end 
		
	end 	

end 

function explodeBehaviour()

	progress_percent = 1.0 * explode_frame / explode_frame_max
	scale = 15.0
	
	if(progress_percent <= 0.5)then 
		SetScale(my_id, 1 + scale * progress_percent * 2)
	else 
		SetScale(my_id, 1 + scale * (1 - progress_percent))
	end 		

	
	--Kill player on contact
	possibly_player = CheckCollisionWithTag(my_id,"Player",0,0,0)
	if(possibly_player > -1)then 
		EnemyHit("Bullet")
		Disable(my_id)
	end 	
			
	if(progress_percent > 1)then 
		Disable(my_id)
	end
		
	explode_frame = explode_frame + 1
	
	return 

end 


function message()
	m0 = GetMessageParameterString(0)
	
	if m0 == "SetSpeed" then 
		speed = GetMessageParameterString(1)
	end --SpeedMult

	
	
	if m0 == "YourTarget" then
		m1 = GetMessageParameterString(1)
		if(m1 ~= nil)then 
			target_x,target_y = GetPosition(m1)
			target_available = true
		end
	end --SpeedMult
	
	if m0 == "YourTargetXY" then
		target_x = GetMessageParameterString(1)
		target_y = GetMessageParameterString(2)
		if(target_y ~= nil)then 
			target_available = true
		end
		
	end --SpeedMult	
	
	if m0 == "YourFractureTarget" then
		m1 = GetMessageParameterString(1)
		if(m1 ~= nil)then 
			fracture_ids[#fracture_ids + 1] = m1
		end
	end --SpeedMult
	
	
	if m0 == "ExplodeAtTheEnd" then
		explode_at_end = 1
	end --SpeedMult
		
		
	
end 
