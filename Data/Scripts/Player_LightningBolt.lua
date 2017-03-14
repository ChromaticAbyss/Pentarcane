script_name = "Player_LightningBolt.lua"
my_id = -1
--Specialised
speed = 0.20
sound_frame_cd = 0

function setup()
	my_id = GetMyId() 
	
end



function frame()
	sound_frame_cd = sound_frame_cd - 1

		
	dx,dy = GetInputs()
	dx = dx * speed
	dy = dy * speed

	sparky_sound = false
	
	for ii=0,2 do
		has_moved = false
		--Horizontal
		if(dx ~=0 and CheckCollisionWithTag(my_id,"Wall",dx,0,0)<=-1)then 
			Move(my_id,dx,0,0)
			has_moved = true
		else
			if(dx~=0) then sparky_sound = true end 
		end 
		--Vertical
		if(dy ~= 0 and CheckCollisionWithTag(my_id,"Wall",0,dy,0)<=-1)then 
			Move(my_id,0,dy,0)
			has_moved = true
		else
			if(dy~=0) then sparky_sound = true end 
		end 


		
		
		if(dx ~= 0)then 
			--Horzontal (and slightly vertical)
			if(has_moved==false and CheckCollisionWithTag(my_id,"Wall",dx,dx/4,0)<=-1)then 
				Move(my_id,dx,dx/4,0)
				has_moved = true		
			end 
			if(has_moved==false and CheckCollisionWithTag(my_id,"Wall",dx,-dx/4,0)<=-1)then 
				Move(my_id,dx,-dx/4,0)
				has_moved = true		
			end 
		end 
		if(dy ~= 0)then 
			--Vertical and slightly sideways
			if(has_moved==false and CheckCollisionWithTag(my_id,"Wall",dy/4,dy,0)<=-1)then 
				Move(my_id,dy/4,dy,0)
				has_moved = true		
			end 
			if(has_moved==false and CheckCollisionWithTag(my_id,"Wall",-dy/4,dy,0)<=-1)then 
				Move(my_id,-dy/4,dy,0)
				has_moved = true		
			end 
		end 
	
	end 
	
	
	--Play this sound when we can't move, or (positioning between the two checks) when we scrape along something
	--if(dx < -0.1 or dx>0.1 or dy<-0.1 or dy>0.1)then
	--	if(has_moved == false)then
			if(sparky_sound==true)then	
				random_lightning_sound()
			end 
	--	end 
	--end 
	
end 



function random_lightning_sound()

	if(sound_frame_cd > 0)then
		return 
	end 
	sound_frame_cd = math.random(12,35)
    ra_a = math.random(2)
	if(ra_a==1)then 
		PlaySound("Zap1.ogg")
	end 
	if(ra_a==2) then 
		PlaySound("Zap2.ogg")
	end 
end 

