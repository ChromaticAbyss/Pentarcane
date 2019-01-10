script_name = "Pickup_Shield.lua"
my_id = -1

function setup()
	my_id = GetMyId() 
	
end




function frame()
	--Disable self and grant bonus on player contact
	possibly_player = CheckCollisionWithTag(my_id,"Player",0,0,0)
	if(possibly_player > -1)then 
		GrantLabyrinthBonus("ExtraDamage_1",GrantVariantReward(),"+1 Damage")
		ScreenShake(0.05)
		PlaySound("Powerup1.ogg")
		Disable(my_id)
	end 
	RotateZ(my_id,0.8)
end 


function GrantVariantReward()  
	variant =  my_id % 10 + 1
	
	if(variant == 1) then
		return "sa"
	end 
	if(variant == 2) then
		return "ra"
	end 
	if(variant == 3) then
		return "ge"
	end 	
	if(variant == 4) then
		return "is"
	end 	
	if(variant == 5) then
		return "me"
	end 		
	if(variant == 6) then
		return "se"
	end 		
	if(variant == 7) then
		return "ky"
	end 		
	if(variant == 8) then
		return "ru"
	end 				
	if(variant == 9) then
		return "je"
	end 		
	if(variant == 10) then
		return "ea"
	end 		
end
