script_name = "Player_FrozenStar.lua"
my_id = -1
--Specialised
speed = 0.15
follower_orbit_distance = 1.3
follower_orbit_angular_speed = 0.04
followers = {}

nr_followers = 8
skip_spawn_frame = 1

function setup()
	my_id = GetMyId() 
	my_x,my_y = GetPosition(my_id)
	
	for ii=1,nr_followers do
		followers[ii] = {id=Spawn("Player_FrozenStar_MindlessFollower",my_x,my_y,0), angle = 360*ii/nr_followers}
	end 

	for jj=1,(nr_followers * follower_distance_frames) do
		recent_positions[jj] = {x=my_x, y=my_y}
	end 	
end

function frame()
	if(skip_spawn_frame == 1)then --We skip the first frame to allow the sub objects to be added properly!
		skip_spawn_frame = 0 
		return
	end


	dx,dy = GetInputs()
	dx = dx * speed
	dy = dy * speed
	

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
	

	--Code regarding followers
	if(dx~=0 or dy~=0)then
		my_x,my_y = GetPosition(my_id)
		
		for ii=1,nr_followers do
			angle = followers[ii].angle
			angle = angle + follower_orbit_angular_speed
			dx = follower_orbit_distance * math.sin(angle)
			dy = follower_orbit_distance * math.cos(angle)
			
			MoveTo(followers[ii].id,my_x + dx,my_y + dy,0)
			
			followers[ii].angle = angle
		end 
	end 
	
end 


function message()
	m0 = GetMessageParameterString(0)

	if m0 == "SpeedMult" then 
		speed = speed * ( GetMessageParameterString(1))
	end --SpeedMult

	
	if m0 == "Die" then 
		--if(intercept_disable()==0)then
		--	Disable(my_id)
		--end	
	end --SpeedMult
	
end 
