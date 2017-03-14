script_name = "ExplodingGroundTile_Explosion.lua"
my_id = -1
my_x = -10
my_y = -10

state = 0 --0 down, 1 moving down, 2 moving up, 3 up(and dangerous)
progress = 0 --out of 10

steps = 6

function setup()
	my_id = GetMyId() 
	my_x,my_y = GetPosition(my_id)
	Hide(my_id)
end

function frame()

	if(state==1)then
		--Kill player on contact, spikes are only dangerous in extended mode
		possibly_player = CheckCollisionWithTag(my_id,"Player",0,0,0)
		if(possibly_player > -1)then 
			--Disable(possibly_player)--TODO: make this a message!
			SendMessage(possibly_player,0,"Die")
		end 
		
		progress = progress + 1
		if(progress > steps)then 
			Hide(my_id)
			state = 0
		end
			
	end 
end 


function message()
	m0 = GetMessageParameterString(0)
	
	if m0 == "Flash" then 
		PlaySound("ObeliskImpact.wav")
		Unhide(my_id)
		state = 1
		progress = 0
	end --PopUp

	
end 