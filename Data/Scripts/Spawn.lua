script_name = "Spawn.lua"
my_id = -1

did_spawning = 0

spell_type = -1 

function setup()
	my_id = GetMyId() 
	spell_type = GetSpellType()
end

function frame()
	if(did_spawning==0)then 
		x,y,z = GetPosition(my_id)
		
		if(spell_type<=1)then
			Spawn("Player_Fireball",x,y,z)
		end
		if(spell_type==2)then 
			Spawn("Player_LightningBolt",x,y,z)
		end 
		if(spell_type==3)then 
			Spawn("Player_Sliding",x,y,z)
		end 
		if(spell_type==4)then 
			Spawn("Player_Lifesteal_A",x,y,z)
		end 	
		did_spawning = 1
		Hide(my_id)
		
		--SendMessage(my_id,0,"Test_msg","Test_msg_2","Test_msg_3") --Test message system
	end

	--Spell 4: Drain life needs to return to the origin to win!
	if(spell_type==4)then 	
		test_id = GetIdFromUniqueName("Player_OnReturn")	

		if(test_id >= 0)then
			Unhide(my_id)
		end 
		possibly_player = CheckCollisionWithTag(my_id,"Player",0,0,0)
		if(possibly_player > -1)then 
			if(test_id == possibly_player)then 
				GrantLabyrinthGoal()
				Disable(possibly_player)			
			end 
		end 
	end 
end 

function message()

end 