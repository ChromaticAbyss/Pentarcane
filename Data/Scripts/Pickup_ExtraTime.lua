script_name = "Pickup_ExtraTime.lua"
my_id = -1

function setup()
	my_id = GetMyId() 
	
end

function frame()
	--Disable self and grant bonus on player contact
	possibly_player = CheckCollisionWithTag(my_id,"Player",0,0,0)
	if(possibly_player > -1)then 
		--PlaySound("../Data/Sounds/magicsonar.wav")
		GrantLabyrinthBonus("ExtraTime","te'","Stun-timer reset")
		ScreenShake(0.1)
		Disable(my_id)
	end 
	RotateZ(my_id,0.8)
end 
