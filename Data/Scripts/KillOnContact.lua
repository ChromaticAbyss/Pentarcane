script_name = "KillOnContact.lua"
my_id = -1

function setup()
	my_id = GetMyId() 
	
end

function frame()
	possibly_player = CheckCollisionWithTag(my_id,"Player",0,0,0)
	if(possibly_player > -1)then 
		--Disable(possibly_player)
		SendMessage(possibly_player,0,"Die")
	end 		
	
end 
