script_name = "ExplodingGroundTile_Explosion.lua"
my_id = -1
my_x = -10
my_y = -10

state = 0 --0 down, 1 on
progress = 0 --out of (steps)
hit_player = 0

steps = 10

function setup()
	my_id = GetMyId() 
	my_x,my_y = GetPosition(my_id)
	SetScale(my_id,0.1)	

	Hide(my_id)
end

function frame()

	if(state==1)then

		progress = progress + 1
		progress_percent = 1.0 * progress / steps
		if(progress_percent <= 0.5)then 
			SetScale(my_id,progress_percent * 2)
		else 
			SetScale(my_id,1 - (progress_percent))	
		end 
		
		
		--Kill player on contact, spikes are only dangerous in extended mode
		possibly_player = CheckCollisionWithTag(my_id,"Player",0,0,0)
		if(possibly_player > -1)then 
			--Disable(possibly_player)--TODO: make this a message!
			if(hit_player == 0)then 
				SendMessage(possibly_player,0,"Boop") 
				EnemyHit("Obelisk Explosion");
				hit_player = 1
				Hide(my_id)
			end 
			--EnemyPlayerCollision("Explosion")
		end 
		
		if(progress > steps)then 
			Hide(my_id)
			state = 0
		end
			
	end 
end 


function message()
	m0 = GetMessageParameterString(0)
	
	if m0 == "Flash" then 
		hit_player = 0
		PlaySound("ObeliskImpact.wav")
		Unhide(my_id)
		state = 1
		progress = 0
	end --PopUp

	
end 