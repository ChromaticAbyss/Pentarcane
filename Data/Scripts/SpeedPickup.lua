script_name = "SpeedPickup.lua"
my_id = -1

function setup()
	my_id = GetMyId() 
end

function frame()
	possibly_player = CheckCollisionWithTag(my_id,"Player",0,0,0)
	if(possibly_player > -1)then 
		SendMessage(possibly_player,0,"SpeedMult",1.5) --Message player object to accelerate itself by 50%
		PlaySound("Powerup1.ogg")
		ScreenShake(0.05)
		Disable(my_id)
	end 		
	
end 



