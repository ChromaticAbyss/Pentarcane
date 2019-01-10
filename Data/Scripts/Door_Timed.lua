script_name = "Door_Timed.lua"
my_id = -1


my_blockade_id = -1
triggered = 0
open = 0
open_frames = 300 --Parameter[0]
countdown = 0

frozen = 0

function setup()
	my_id = GetMyId() 
	Hide(my_id,1)
	Hide(my_id,2)
	Hide(my_id,3)
	Hide(my_id,4)--ice crystal means the gate is permanently locked open
	
	x,y = GetPosition(my_id)
	
	my_blockade_id = Spawn("Door_Timed_Blockade",x,y,0)--Remember: Don't use this frame	
	SendMessage(my_blockade_id,0,"Down")			
	
	open_frames = GetScriptParameterFloat(0) or open_frames --Parameter[0]: Nr of frames the door opens for when triggered
end

function frame()
	if(frozen == 1)then 
		RotateZ(my_id,-1,4)	
	end 

	if(triggered == 1 and frozen==0)then 
		spell_type = GetSpellType()
		if(spell_type==3)then 
			--Ice crystal: Freeze spell permanently opens these
			frozen = 1
			Unhide(my_id,4)
			PlaySound("Unlock.ogg")
			triggered = 0
			SendMessage(my_blockade_id,0,"Up")	
				return
		end 	
		
		
		Unhide(my_id,1)
		Hide(my_id,2)
		Hide(my_id,3)
		open = 1
		PlaySound("Unlock.ogg")
		triggered = 0
		countdown = open_frames
		SendMessage(my_blockade_id,0,"Up")
		--Send message to blockade object

		
	end

	if(open == 1)then
		countdown = countdown - 1
		
		RotateZ(my_id,-1,1)
		RotateZ(my_id,-1,2)
		RotateZ(my_id,-1,3)
		if(countdown == math.floor(open_frames * 0.35))then 
			Hide(my_id,1)
			Unhide(my_id,2)
		end 
		if(countdown == math.floor(open_frames * 0.10))then 
			Hide(my_id,2)
			SetScale(my_id, 1, 3)
			Unhide(my_id,3)
		end 				
		
		if(countdown<=0)then
			open = 0
			Hide(my_id,3)
			SendMessage(my_blockade_id,0,"Down")
			--Unhide(my_id,1)
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
