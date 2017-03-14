script_name = "Door.lua"
my_id = -1

open = 0
my_blockade_id = -1;

function setup()
	my_id = GetMyId() 
	
	x,y = GetPosition(my_id)
	
	my_blockade_id = Spawn("Door_Blockade",x,y,0)--Remember: Don't use this frame	
end

function frame()

	--Disable self and grant bonus on player contact
	--TODO: Animation that makes the barrier sink into the ground/dissapear otherwise
	if(open==0)then 
		possibly_player = CheckCollisionWithTag(my_id,"Player",0,0,0)
		if(possibly_player > -1)then 
			result = UseLabyrinthKey()
				--print("key?")

			if(result==1)then
				--print("Used key")
				open = 1
				PlaySound("Unlock.ogg")
				Disable(my_blockade_id)
			end 
		end 
	end 
end 
