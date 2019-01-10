script_name = "ExplodingGroundTile_Explosion.lua"
my_id = -1
my_x = -10
my_y = -10

state = 0 --0 down, 1 moving down, 2 moving up, 3 up(and dangerous), 10 hit player (disabled)
progress = 0 --out of 10

retract_how_far = -1.001
steps = 6

function setup()
	my_id = GetMyId() 
	my_x,my_y = GetPosition(my_id)
	Hide(my_id)
	MoveTo(my_id,my_x,my_y,retract_how_far)
end

function frame()
	if(state==1)then 
		progress = progress + 1 	
		MoveTo(my_id,my_x,my_y,retract_how_far * progress / steps)
		if(progress >= steps)then 
			Hide(my_id)
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
		EnemyPlayerCollision("Spike")
		
	end 
end 


function message()
	m0 = GetMessageParameterString(0)
	
	if m0 == "PopUp" then 
		Unhide(my_id)
		state = 2
		progress = 0
	end --PopUp

	
	if m0 == "Retract" then 
		state = 1
		progress = 0
	end --Retract
	
end 