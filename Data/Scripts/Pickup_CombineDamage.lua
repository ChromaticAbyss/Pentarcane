script_name = "Pickup_Shield.lua"
my_id = -1

function setup()
	my_id = GetMyId() 
	
end

function frame()
	--Disable self and grant bonus on player contact
	possibly_player = CheckCollisionWithTag(my_id,"Player",0,0,0)
	if(possibly_player > -1)then 
		GrantLabyrinthBonus("CombineDamage",GrantVariantReward(),"Combo: +5 damage (1/5)")
		PlaySound("Powerup1.ogg")
		ScreenShake(0.1)
		Disable(my_id)
	end 
	RotateZ(my_id,0.8)
end 


function GrantVariantReward()  
	variant =  my_id % 10 + 1
	
	if(variant == 1) then
		return "a-es"
	end 
	if(variant == 2) then
		return "e-ak"
	end 
	if(variant == 3) then
		return "i-ve"
	end 	
	if(variant == 4) then
		return "mi-ra"
	end 	
	if(variant == 5) then
		return "s-ti"
	end 		
	if(variant == 6) then
		return "mi-no"
	end 		
	if(variant == 7) then
		return "ce-ra"
	end 		
	if(variant == 8) then
		return "an-as"
	end 				
	if(variant == 9) then
		return "a-ak"
	end 		
	if(variant == 10) then
		return "e-eir"
	end 		
end


