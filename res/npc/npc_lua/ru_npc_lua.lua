-- Routine for NPC "Lua"

loadRoutine = function(R, W)

	R:setTilePosition(20,2.5)
	
	R:setFacingUp()
	R:wait(10000)

	R:goToTile(22,2.5)
	R:setFacingUp()
	R:wait(7000)
	R:goToTile(23,3)
	R:goToTile(23,3.5)
	R:setFacingLeft()
	R:wait(12000)
	R:goToTile(20,4)
	R:goToTile(20,7)
	R:goToTile(12,7)
	R:goToTile(12,12)
	R:goToTile(21,12)
	R:goToTile(21,10.5)
	R:setFacingUp()
	R:wait(6000)
	R:goToTile(21,12)
	R:goToTile(12,12)
	R:goToTile(12,7)
	R:goToTile(20,7)
	R:goToTile(20,4)
	R:goToTile(19,4)
	R:goToTile(19,3.3)
	R:setFacingRight()
	R:wait(10000)
	R:goToTile(20,3.3)
	R:goToTile(20,2.5)
end	