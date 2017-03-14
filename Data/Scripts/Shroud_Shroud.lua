script_name = "Shroud_Shroud.lua"
my_id = -1

--Specialised

triggered = 0

function setup()
	my_id = GetMyId() 
	my_x,my_y = GetPosition(my_id)
		
end

function frame()

	
	if(triggered == 1)then 
		Disable(my_id)
		triggered = 0
	end 


	
end 



function message()
	m0 = GetMessageParameterString(0)
	
	if m0 == "Dissappear" then 
		triggered = 1
	end --SpeedMult

end 
