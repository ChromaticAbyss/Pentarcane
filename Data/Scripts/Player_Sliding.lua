script_name = "Player_Sliding.lua"
my_id = -1
--Specialised
speed = 0.20
speed_falloff = 0.0008
acceleration = 0.015

dx = 0
dy = 0

function setup()
	my_id = GetMyId() 
	
end

function frame()
	n_dx,n_dy = GetInputs()
	dx = dx + n_dx * acceleration
	dy = dy + n_dy * acceleration
	bounce_x = false
	bounce_y = false
	
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
	else 
		bounce_x = true
	end 
	--Vertical
	if(CheckCollisionWithTag(my_id,"Wall",0,dy,0)<=-1)then 
		Move(my_id,0,dy,0)
		has_moved = true
	else
		bounce_y = true
	end 

	if(dx ~= 0)then 
		--Horzontal (and slightly vertical)
		if(has_moved==false and CheckCollisionWithTag(my_id,"Wall",dx,dx/4,0)<=-1)then 
			Move(my_id,dx,dx/4,0)
			has_moved = true	
			bounce_x = false
		end 
		if(has_moved==false and CheckCollisionWithTag(my_id,"Wall",dx,-dx/4,0)<=-1)then 
			Move(my_id,dx,-dx/4,0)
			has_moved = true	
			bounce_x = false			
		end 
	end 
	if(dy ~= 0)then 
		--Vertical and slightly sideways
		if(has_moved==false and CheckCollisionWithTag(my_id,"Wall",dy/4,dy,0)<=-1)then 
			Move(my_id,dy/4,dy,0)
			has_moved = true	
			bounce_y = false			
		end 
		if(has_moved==false and CheckCollisionWithTag(my_id,"Wall",-dy/4,dy,0)<=-1)then 
			Move(my_id,-dy/4,dy,0)
			has_moved = true
			bounce_y = false			
		end 
	end 
	
	--Bounce off walls
	if(bounce_x==true)then 
		print("bounced")
		dx = - 1 * dx/2
	end 
	if(bounce_y==true) then
		dy = - 1 * dy/2
	end 
	
end 


