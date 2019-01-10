script_name = "SlowMine.lua"
my_id = -1

function setup()
	my_id = GetMyId() 
end

function frame()
	possibly_player = CheckCollisionWithTag(my_id,"Player",0,0,0)
	if(possibly_player > -1)then 
		SendMessage(possibly_player,0,"SpeedMult",0.7) --Message player object to slow itself by 30%
		PlaySound("slow_mine.ogg")
		ScreenShake(0.1)
		Disable(my_id)
	end 		
	
end 



