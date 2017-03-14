


function message()
	m0 = GetMessageParameterString(0)
	
	if m0 == "SpeedMult" then 
		speed = speed * ( GetMessageParameterString(1))
	end --SpeedMult

	
	
	if m0 == "Die" then
		PlaySound("labyrinth_fail.ogg")	
		Disable(my_id)
	end --SpeedMult
end 
