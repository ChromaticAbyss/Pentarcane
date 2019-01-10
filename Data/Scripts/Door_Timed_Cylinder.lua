script_name = "Door_Timed_Cylinder.lua"
my_id = -1
my_x = -10
my_y = -10

state = 0 --0 open, 1 moving up, 2 moving down, 3 closed(and dangerous)
progress = 0 --out of 10

retract_how_far = 1.00
steps_to = 6

step = 0

function setup()
	my_id = GetMyId() 
	my_x,my_y = GetPosition(my_id)
	MoveTo(my_id,my_x,my_y,0)
end

function frame()
	step = step + 1
	

	
	if(state==1)then 
		progress = progress + 1 	
		MoveTo(my_id,my_x,my_y,retract_how_far * progress / steps_to)
		if(progress >= steps_to)then 
			state = 0
			progress = 0
		end 
		SetColor(my_id,0.5,1,1)
	end 

	
	if(state==2)then 
		progress = progress + 1
		MoveTo(my_id,my_x,my_y,retract_how_far * (steps_to - progress) / steps_to)
		if(progress >= steps_to)then 
			ScreenShake(0.1)
			state = 3
			progress = 0
		end 
		SetColor(my_id,0.5,1,1)
	end 

	if(state==3)then
		--Kill player on contact, spikes are only dangerous in extended mode
		EnemyPlayerCollision("Death")
		EnemyDeathPulsing(step)
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