script_name = "Pickup_Shield.lua"
my_id = -1

function setup()
	my_id = GetMyId() 
	
end

function frame()
	--Disable self and grant bonus on player contact
	possibly_player = CheckCollisionWithTag(my_id,"Player",0,0,0)
	if(possibly_player > -1)then 
		GrantLabyrinthBonus("Shield",GrantVariantReward(),"Grants 3 points of shield")
		PlaySound("Powerup1.ogg")
		ScreenShake(0.05)
		Disable(my_id)
	end 
	RotateZ(my_id,0.8)
end 


function GrantVariantReward()  
	variant =  my_id % 10 + 1
	
	if(variant == 1) then
		return "assa"
	end 
	if(variant == 2) then
		return "lyr"
	end 
	if(variant == 3) then
		return "kah"
	end 	
	if(variant == 4) then
		return "essa"
	end 	
	if(variant == 5) then
		return "draka"
	end 		
	if(variant == 6) then
		return "pha"
	end 		
	if(variant == 7) then
		return "tham"
	end 		
	if(variant == 8) then
		return "dre"
	end 				
	if(variant == 9) then
		return "mar"
	end 		
	if(variant == 10) then
		return "arc"
	end 		
end
