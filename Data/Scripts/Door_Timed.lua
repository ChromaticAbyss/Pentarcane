script_name = "Door_Timed.lua"
my_id = -1


my_blockade_id = -1
triggered = 0
open = 0
open_frames = 300 --Parameter[0]
countdown = 0

function setup()
	my_id = GetMyId() 
	
	x,y = GetPosition(my_id)
	
	my_blockade_id = Spawn("Door_Timed_Blockade",x,y,0)--Remember: Don't use this frame	
	SendMessage(my_blockade_id,0,"Down")			
	
	open_frames = GetScriptParameterFloat(0) or open_frames --Parameter[0]: Nr of frames the door opens for when triggered
end

function frame()
	if(triggered == 1)then 
		open = 1
		PlaySound("Unlock.ogg")
		triggered = 0
		countdown = open_frames
		SendMessage(my_blockade_id,0,"Up")
		--Send message to blockade object
	end

	if(open == 1)then
		countdown = countdown - 1
		if(countdown<=0)then
			open = 0
			SendMessage(my_blockade_id,0,"Down")
			PlaySound("Lock.ogg")
		end
	end 

end 


function message()
	m0 = GetMessageParameterString(0)
	
	if m0 == "Trigger" then 
		triggered = 1
	end --SpeedMult

end 
