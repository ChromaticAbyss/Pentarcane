script_name = "Switch_Base.lua"
my_id = -1

--Specialised

child_object_indicator = -1
state = 1
delay = 100
delay_left = -1

skip_frame = 1 --To avoid using our spawned id's before they are ready


function setup()
	my_id = GetMyId() 
	my_x,my_y = GetPosition(my_id)
	
	child_object_indicator = Spawn("Switch_IndicatorBlock",my_x,my_y,0)	
	
	delay = GetScriptParameterFloat(0) or delay
	SendMessage(child_object_indicator,0,"PopUp",0.7)
	
	
end

function frame()
	if(skip_frame==1)then
		skip_frame = 0
		return 
	end
	
	delay_left = delay_left - 1
	
	
	if(delay_left < 0)then 
	
		triggered = 0
		possibly_player = CheckCollisionWithTag(my_id,"Player",0,0,0)
		if(possibly_player > -1)then 
			delay_left = delay
			SendMessage(child_object_indicator,0,"Retract",0.7)
			PlaySound("../Data/Sounds/positive_trigger.ogg")
			ScreenShake(0.05)
			--Message all objects named in the signalers string parameters
			count = 0
			name = GetScriptParameterString(0)
			while name ~= nil do
				target_id = GetIdFromUniqueName(name)
				if target_id ~= nil then 
					SendMessage(target_id,0,"Trigger")
				end 
				count = count + 1
				name = GetScriptParameterString(count)
			end 			
			
		end 
		
		
	end 

	if(delay_left == 0)then 
		SendMessage(child_object_indicator,0,"PopUp",0.7)
	end 
	
	
end 



function message()
	m0 = GetMessageParameterString(0)
	
	if m0 == "Trigger" then 
		triggered = 1
	end --SpeedMult

end 
