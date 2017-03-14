script_name = "Player_Fireball.lua"
my_id = -1
--Specialised
speed = 0.15
speed_falloff = 0.035
acceleration = 0.07

dx = 0
dy = 0

function setup()
	my_id = GetMyId() 
	
end

function frame()
	n_dx,n_dy = GetInputs()
	dx = dx + n_dx * acceleration
	if(dx>0)then 
		if(dx>speed_falloff)then 
			dx = dx - speed_falloff
		else
			dx = 0
		end
		if(dx>=speed)then 
			dx=speed
		end 	
	end 
	if(dx<0)then 
		if(dx<-speed_falloff)then 
			dx = dx + speed_falloff
		else
			dx = 0
		end
		if(dx<=-speed)then 
			dx=-speed
		end	
	end 

	dy = dy + n_dy * acceleration
	if(dy>0)then 
		if(dy>speed_falloff)then 
			dy = dy - speed_falloff
		else
			dy = 0
		end
		if(dy>=speed)then 
			dy=speed
		end 	
	end 
	if(dy<0)then 
		if(dy<-speed_falloff)then 
			dy = dy + speed_falloff
		else
			dy = 0
		end
		if(dy<=-speed)then 
			dy=-speed
		end	
	end 
	
	
	has_moved = false
	--Horizontal
	if(CheckCollisionWithTag(my_id,"Wall",dx,0,0)<=-1)then 
		Move(my_id,dx,0,0)
		has_moved = true
	end 
	--Vertical
	if(CheckCollisionWithTag(my_id,"Wall",0,dy,0)<=-1)then 
		Move(my_id,0,dy,0)
		has_moved = true
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


