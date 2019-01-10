script_name = "Pickup_ArmorPierce.lua"
my_id = -1

function setup()
	my_id = GetMyId() 
	
end

function frame()
	--Disable self and grant bonus on player contact
	possibly_player = CheckCollisionWithTag(my_id,"Player",0,0,0)
	if(possibly_player > -1)then 
		GrantLabyrinthBonus("IgnoreArmor","d'ra","Ignores target armor")
		GrantLabyrinthBonus("Shatter","","Reduces target armor by 2")
		PlaySound("Powerup1.ogg")
		Disable(my_id)
		ScreenShake(0.1)
	end 
	RotateZ(my_id,0.8)
end 
