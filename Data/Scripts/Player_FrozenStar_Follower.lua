script_name = "Player_FrozenStar_Follower.lua"
my_id = -1
my_x = -1 
my_y = -1 

function setup()
	my_id = GetMyId() 
	my_x,my_y = GetPosition(my_id)
	
end

function frame()
	
	
end 



function message()
	m0 = GetMessageParameterString(0)

	--if m0 == "SpeedMult" then 
	--	speed = speed * ( GetMessageParameterString(1))
	--end --SpeedMult

	
	if m0 == "Die" then 
		Disable(my_id)
	end --SpeedMult
	
	
	--Many other messages should be forwarded to the core(such as an orbiter hitting a slow mine slows the entire assemble)
	
	
end 
