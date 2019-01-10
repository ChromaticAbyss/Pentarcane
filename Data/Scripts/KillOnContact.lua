script_name = "KillOnContact.lua"
my_id = -1

function setup()
	my_id = GetMyId() 
	
end

function frame()
	possibly_player = CheckCollisionWithTag(my_id,"Player",0,0,0)
	if(possibly_player > -1)then 
		EnemyPlayerCollision("Death")
		EnemyDeathPulsing(step)
	end 		
	
end 
