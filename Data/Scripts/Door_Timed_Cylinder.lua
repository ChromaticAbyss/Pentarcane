script_name = "Door_Timed_Cylinder.lua"
my_id = -1
my_x = -10
my_y = -10

state = 0 --0 open, 1 moving up, 2 moving down, 3 closed(and dangerous)
progress = 0 --out of 10

retract_how_far = 1.00
steps = 6

function setup()
	my_id = GetMyId() 
	my_x,my_y = GetPosition(my_id)
	MoveTo(my_id,my_x,my_y,0)
end

function frame()
	if(state==1)then 
		progress = progress + 1 	
		MoveTo(my_id,my_x,my_y,retract_how_far * progress / steps)
		if(progress >= steps)then 
			state = 0
			progress = 0
		end 
	end 

	
	if(state==2)then 
		progress = progress + 1
		MoveTo(my_id,my_x,my_y,retract_how_far * (steps - progress) / steps)
		if(progress >= steps)then 
			state = 3
			progress = 0
		end 
	end 

	if(state==3)then
		--Kill player on contact, spikes are only dangerous in extended mode
		possibly_player = CheckCollisionWithTag(my_id,"Player",0,0,0)
		if(possibly_player > -1)then 
			--Disable(possibly_player)--TODO: make this a message!
			SendMessage(possibly_player,0,"Die")
		end 
	end 
end 


function message()
	m0 = GetMessageParameterString(0)
	
	if m0 == "Down" then 
		state = 2
		progress = 0
	end --Close

	if m0 == "Up" then 
		state = 1
		progress = 0
	end --Open
	
end 