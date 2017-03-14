script_name = "Obelisk_Ball.lua"
my_id = -1
my_x = -10
my_y = -10

state = 0 --0 invis, 1 moving down
progress = 0 --out of 10

starting_height = 20.0
steps = 100

function setup()
	my_id = GetMyId() 
	my_x,my_y = GetPosition(my_id)
	Hide(my_id)
	MoveTo(my_id,my_x,my_y,starting_height)
end

function frame()
	if(state==1)then 
		progress = progress + 1 	
		MoveTo(my_id,my_x,my_y,starting_height * (1 - progress / steps))
		if(progress >= steps)then 
			Hide(my_id)
			state = 0
			progress = 0
		end 
	end 


end 


function message()
	m0 = GetMessageParameterString(0)
	
	if m0 == "Drop" then 
		Unhide(my_id)
		state = 1
		progress = 0
	end --PopUp
	
end 