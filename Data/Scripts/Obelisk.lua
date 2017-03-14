script_name = "Obelisk.lua"
my_id = -1


--Specialised

child_object_ball = -1
child_object_explosion = -1
state = 0
countdown = 0

skip_frame = 1 --To avoid using our spawned id's before they are ready


function setup()
	my_id = GetMyId() 
	my_x,my_y = GetPosition(my_id)
	
	child_object_ball = Spawn("Obelisk_Ball",my_x,my_y,0)	
	child_object_explosion = Spawn("Obelisk_Explosion",my_x,my_y,0)	
	
	
	
end

function frame()
	if(skip_frame==1)then
		skip_frame = 0
		return 
	end
	
	if(state == 0)then 
		--Kill player on contact
		possibly_player = CheckCollisionWithTag(my_id,"Player",0,0,0)
		if(possibly_player > -1)then 
			--Disable(possibly_player)
			state = 1
			countdown = 100 --Sync with the orange ball falling
			PlaySound("ObeliskTrigger.wav")
			SendMessage(child_object_ball,0,"Drop")
		end 	
	end 
	
	if(state == 1)then 
		countdown = countdown - 1 
		if(countdown <= 0)then 
			state = 0
			SendMessage(child_object_explosion,0,"Flash")
		end 
	end 


	
end 

