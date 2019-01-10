script_name = "Shroud_Shroud.lua"
my_id = -1

--Specialised

triggered = 0 -- 0 , 1 yes, 2 in porcess of disspearing

dissapear_time = 50
frames_since_trigger = 0
function setup()
	my_id = GetMyId() 
	my_x,my_y = GetPosition(my_id)
		
end

function frame()

	
	if(triggered == 1)then 
		triggered = 2
		frames_since_trigger = dissapear_time
	end 

    if(triggered == 2)then 
		frames_since_trigger = frames_since_trigger - 1
		if(frames_since_trigger<=0)then 
			Disable(my_id)
		end 
		SetScale(my_id,frames_since_trigger / dissapear_time)
	end 
			

end 



function message()
	m0 = GetMessageParameterString(0)
	
	if m0 == "Dissappear" then 
		if(triggered == 0)then 
			triggered = 1
		end
	end

end 
