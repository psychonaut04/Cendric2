-- Dialogue for NPC "hunter_edmond"
loadDialogue = function(DL)
		if (not DL:isConditionFulfilled("tutorial_complete")) then
			if (DL:isNPCState("hunter_edmond", "never_talked")) then
				DL:createNPCNode(0, 1, "DL_Edmond_Hello") -- Hey you, stop! What are you doing here, so very alone? It's dangerous out here.
				DL:changeNPCState("hunter_edmond", "passive")
				DL:addNode()
			
				DL:setRoot(0)
			
				DL:createChoiceNode(1)
				DL:addChoice(2, "DL_Choice_Exploring") -- Dunno... Exploring, I guess?
				DL:addChoice(3, "DL_Choice_WhatIsDangerous") --  What exactly is dangerous?
				DL:addNode()
				
				DL:createNPCNode(3, 5, "DL_Edmond_DangerousIs") -- These woods are not safe for a young man like you on his own. There are creatures out here and, in addition, big rocks have fallen down from the hill in the east that are blocking our way back to the village.
				DL:addNode()
				
				DL:createNPCNode(2, 4, "DL_Edmond_JustExploring") -- So, just exploring? It is unusual that I meet someone out here that I don't know. You are not a citizen of the village, otherwise I would have seen you before.
				DL:addNode()
				
				DL:createChoiceNode(4)
				DL:addChoice(6, "DL_Choice_TheVillage") -- What village?
				DL:addChoice(3, "DL_Choice_WhatIsDangerous") --  What exactly is dangerous?
				DL:addNode()
				
				DL:createChoiceNode(5)
				DL:addChoice(6, "DL_Choice_TheVillage") -- What village?
				DL:addChoice(7, "DL_Choice_WhatCanWeDo") --  What can we do about those rocks?
				DL:addNode()
				
				DL:createNPCNode(6, 8, "DL_Edmond_TheVillage") -- A small village in the south. I'm a huntsman with the task of delivering meat to these people. You must have seen it as you've come here! 
				DL:addNode()
				
				DL:createCendricNode(8, 9, "DL_Cendric_Nope") -- I don't recall seeing it...
				DL:addNode()
				
				DL:createNPCNode(9, 10, "DL_Edmond_WouldYouHelp") -- That seems strange to me. But anyway, we're trapped here as long as those rocks block our way out. Would you mind helping me? 
				DL:addNode()
				
				DL:createChoiceNode(10)
				DL:addChoice(7, "DL_Choice_WhatCanWeDo") --  What can we do about those rocks?
				DL:addChoice(12, "DL_Choice_IWillHelp") -- Sure! Tell me what to do
				DL:addChoice(11, "DL_Choice_IWontHelp") --  No, that's your problem
				DL:addNode()
				
			elseif (not DL:isConditionFulfilled("talked_to_edmond")) then
				
				DL:createChoiceNode(50)
				DL:addChoice(7, "DL_Choice_WhatCanWeDo") --  What can we do about those rocks?
				DL:addChoice(-1, "DL_Choice_CU") --  See you later
				DL:addNode()
				
				DL:setRoot(50)
				
			end
				
			DL:createNPCNode(7, 13, "DL_Edmond_WeCanCrush") -- They are not made of very hard material. It may be enough to beat our way through. Would you come with me?
			DL:addNode()
			
			DL:createChoiceNode(13)
			DL:addChoice(12, "DL_Choice_IWillHelp") -- Sure! Tell me what to do
			DL:addChoice(11, "DL_Choice_IWontHelp") --  No, that's your problem
			DL:addNode()
			
			DL:createNPCNode(11, -1, "DL_Edmond_YouWontHelp") -- It is not only my problem. Come back to me if you've changed your mind.
			DL:addNode()

						
			DL:createNPCNode(12, -1, "DL_Edmond_YouWillHelp") -- That's great! You could crush some rocks with your fists, but having a weapon is way more comfortable. Here, take my walking pole, equip it and follow me.
			DL:addConditionProgress("talked_to_edmond")
			DL:addItem("we_walkingpole", 1)
			-- TODO screen overlay: equip
			DL:addNode()
				
		else
			DL:createChoiceNode(0)
			DL:addChoice(-1, "DL_Choice_CU") --  See you later
			DL:addNode()
			
			DL:setRoot(0)
		end
	
    end