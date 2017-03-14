script_name = "Shooter.lua"
my_id = -1


--Specialised
frames_between_shots = 60 --Parameter[0]
frames_until_next_shot = 60

operation_mode = 0 --Parameter[1] 0 alternating targets, 1 fragmenting,


target_ids = {}
nr_target_ids = 0

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
		
end

function frame()
	frames_until_next_shot = frames_until_next_shot - 1
	if(frames_until_next_shot>0)then 
		return 
	end 

	frames_until_next_shot = frames_between_shots
	
	if(operation_mode==0)then 
		shootSimple()
	end
	if(operation_mode==1)then 
		shootFracturing()
	end 


	
end 

current_targ_id = 1 --used only in simple shooting mode
function shootSimple()

	new_proj = Spawn("Shooter_Bullet",my_x,my_y,0)	
	SendMessage(new_proj,0,"YourTarget",target_ids[current_targ_id])
	--TODO: Face that direction
	
	current_targ_id = current_targ_id + 1
	if(current_targ_id > nr_target_ids)then 
		current_targ_id = 1
	end
end 


function shootFracturing()

	new_proj = Spawn("Shooter_Bullet",my_x,my_y,0)	
	SendMessage(new_proj,0,"YourTarget",target_ids[1])
	
	for i=2,nr_target_ids,1 do
		SendMessage(new_proj,0,"YourFractureTarget",target_ids[i])	
	end 

end 
