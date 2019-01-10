script_name = "Shooter.lua"
my_id = -1


--Specialised
frames_between_shots = 60 --Parameter[0], -1 or less means triggered only 
frames_until_next_shot = 60

operation_mode = 0 --Parameter[1] 0 alternating targets, 1 fragmenting, 2 aim at player in box

bullet_speed = 0.25 --Parameter[2] 

target_ids = {}
nr_target_ids = 0

triggered = 0

bullets_explode = 0

--operation mode: targetting player
box_x_small = 0
box_x_large = 0
box_y_small = 0
box_y_large = 0
start_delay = 5
player_id = -1

function setup()
	my_id = GetMyId() 
	my_x,my_y = GetPosition(my_id)	
	
	--Targets
	count = 0
	name = GetScriptParameterString(0)
	while name ~= nil do
		target_id = GetIdFromUniqueName(name)
		if target_id ~= -1 then 
			target_ids[1 + #target_ids]=target_id
		end 
		count = count + 1
		name = GetScriptParameterString(count)
	end 
	nr_target_ids = #target_ids
	
	--Behaviour
	frames_between_shots = GetScriptParameterFloat(0) or frames_between_shots
	if(frames_between_shots == 0) then frames_between_shots = 60 end 
	frames_until_next_shot = frames_between_shots
	
	operation_mode = GetScriptParameterFloat(1) or operation_mode	
		mode_overwrite = GetNamedParameter(my_id,"OperationMode")
		if(mode_overwrite == "Swap")then 
			operation_mode = 0
		elseif(mode_overwrite == "Fracture")then 
			operation_mode = 1
		elseif(mode_overwrite == "Aim")then 
			operation_mode = 2
		end			
	
	
	bullets_explode = GetNamedParameter(my_id,"BulletsExplode") or 0
	
	
	bullet_speed = GetScriptParameterFloat(2) or bullet_speed
		
	if(operation_mode == 2)then 
		for i, id in ipairs(target_ids) do
			x,y = GetPosition(id)
			if(x < box_x_small)then 
				box_x_small = x
			end 
			if(y < box_y_small)then 
				box_y_small = y
			end			
			if(x > box_x_large)then 
				box_x_large = x
			end
			if(y > box_y_large)then 
				box_y_large = y
			end			
		end 
	
	end 
		
	
	

	
end

function frame()
	if(operation_mode == 2)then 
		start_delay = start_delay - 1 	
		if(start_delay > 0)then 
			return
		end 
		if(start_delay == 0) then 
			player_id = GetIdFromUniqueName("Player")
		end 
	end 
	
	EnemyPlayerCollision("Shooter")
		
	
	if(triggered == 1)then 
		doShoot()
		triggered = 0
		return 
	end
	
	if(frames_between_shots <= -1)then --Triggered only
		return 
	end
	frames_until_next_shot = frames_until_next_shot - 1
	if(frames_until_next_shot>0)then 
		return 
	end 
	frames_until_next_shot = frames_between_shots
	
	doShoot()
end 

function doShoot() 
	if(operation_mode==0)then 
		shootSimple()
	end
	if(operation_mode==1)then 
		shootFracturing()
	end 
	if(operation_mode==2)then 
		shootAimed()
	end 
end 

function setOtherBulletProperties(new_proj)
	SendMessage(new_proj,0,"SetSpeed",bullet_speed)
	if(bullets_explode ~= 0 and bullets_explode ~= "")then 
		SendMessage(new_proj,0,"ExplodeAtTheEnd")
	end 
end 




current_targ_id = 1 --used only in simple shooting mode
function shootSimple()

	new_proj = Spawn("Shooter_Bullet",my_x,my_y,0)	
	setOtherBulletProperties(new_proj)

	SendMessage(new_proj,0,"YourTarget",target_ids[current_targ_id])
	tx,ty = GetPosition(target_ids[current_targ_id])
	FaceDirection(my_id,tx-my_x,ty-my_y,0);
	
	current_targ_id = current_targ_id + 1
	if(current_targ_id > nr_target_ids)then 
		current_targ_id = 1
	end
end 


function shootFracturing()

	new_proj = Spawn("Shooter_Bullet",my_x,my_y,0)	
	setOtherBulletProperties(new_proj)

	SendMessage(new_proj,0,"YourTarget",target_ids[1])
	tx,ty = GetPosition(target_ids[1])
	FaceDirection(my_id,tx-my_x,ty-my_y,0);
	
	for i=2,nr_target_ids,1 do
		SendMessage(new_proj,0,"YourFractureTarget",target_ids[i])	
	end 

end 


function shootAimed()

	new_proj = Spawn("Shooter_Bullet",my_x,my_y,0)	
	setOtherBulletProperties(new_proj)
		
	tx,ty = GetPosition(player_id)
	dx = tx-my_x
	dy = ty-my_y
	FaceDirection(my_id,dx,dy,0);

	
	cand_len = -1000
	--Caculate target coordinates so we hit the border of the box
	dist_wall_r = dx / (box_x_large - my_x)
	if(dist_wall_r > 0)then 
		if(dist_wall_r > cand_len) then 
			cand_len = dist_wall_r
		end 
	end 
	dist_wall_l = dx / (box_x_small - my_x)
	if(dist_wall_l > 0)then 
		if(dist_wall_l > cand_len) then 
			cand_len = dist_wall_l
		end 
	end 	
	
	
	dist_wall_t = dy / (box_y_large - my_y)
	if(dist_wall_t > 0)then 
		if(dist_wall_t > cand_len) then 
			cand_len = dist_wall_t
		end 
	end 
	dist_wall_b = dy / (box_y_small - my_y)
	if(dist_wall_b > 0)then 
		if(dist_wall_b > cand_len) then 
			cand_len = dist_wall_b
		end 
	end 	
	
		
	
	--act_len = Sqrt(dx * dx + dy * dy)	
	dx = dx / cand_len
	dy = dy / cand_len	
	SendMessage(new_proj,0,"YourTargetXY",my_x + dx , my_y + dy)
end 




function message()
	m0 = GetMessageParameterString(0)
	
	if m0 == "Trigger" then 
		triggered = 1
	end --SpeedMult

	
	

	
end 

