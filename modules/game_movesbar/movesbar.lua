movesbar = {}
movesWindow = nil
-- moves = {}

function init()
	connect(g_game, { --onGameStart = movesbar.create,
	onPokemonMovesCooldown = movesbar.movesSetMovesCooldown,
	onPokemonMoveCooldown = movesbar.movesSetCooldown,
	onGameEnd = movesbar.destroy })		
end

function terminate()
	disconnect(g_game, { --onGameStart = movesbar.create,
	onPokemonMovesCooldown = movesbar.movesSetMovesCooldown,
	onPokemonMoveCooldown = movesbar.movesSetCooldown,
	onGameEnd = movesbar.destroy })
	movesbar.destroy()
	movesbar = nil
end
function movesbar.create()
	movesWindow = g_ui.displayUI('movesbar.otui')
end

function movesbar.destroy()
	if movesWindow then
		movesWindow:setVisible(false)
		movesWindow = nil
		-- moves = {}
	end
end


function movesbar.useMove(n)
	g_game.talk("m"..n)
end

function movesbar.movesSetCooldown(id, cd)
	if not movesWindow then
		return false
	end

	local progress = movesWindow:getChildById('pg'..id)
	local label = movesWindow:getChildById('cd'..id)

	updateCooldown(progress,label, cd, cd, false)
end

function movesbar.movesSetMovesCooldown(moveList)
	if (#moveList > 0) then
		movesbar.create()
	else
		return movesbar.destroy()
	end	
	
	addEvent(function() g_effects.fadeIn(movesWindow, 100) end)
	
	movesWindow:setWidth((#moveList*(32+2))+5)
	for i=1,#moveList do
		local Moveicon = g_ui.createWidget('Icon', movesWindow)
		local MoveProgress = g_ui.createWidget('Progress', movesWindow)
		local MoveCd = g_ui.createWidget('Text', movesWindow)
		local localPlayer = g_game.getLocalPlayer()
		Moveicon:setId("ic"..moveList[i][1])
		MoveProgress:setTooltip(moves[moveList[i][1]].name)
		
		if (not localPlayer:isPremium() and moveList[i][3] == 1) or (localPlayer:getLevel() < moveList[i][4]) then
			Moveicon:setImageSource('/images/game/moves/'..moveList[i][1]..'_off')
		else
			Moveicon:setImageSource('/images/game/moves/'..moveList[i][1]..'_on')
		end

		MoveProgress:setId("pg"..moveList[i][1])
		MoveCd:setId("cd"..moveList[i][1])
		Moveicon:setMarginTop(4)
		marginLeft = 3
		if i ~= 1 then
			marginLeft = marginLeft+((i-1)*(32+2))
		end
		Moveicon:setMarginLeft(marginLeft)
		Moveicon:setColor(white)
		--
		MoveCd:fill(Moveicon:getId())
		MoveCd:setText("")
		--
		MoveProgress:fill(Moveicon:getId())
		MoveProgress.onClick = function() movesbar.useMove(i) end
		if localPlayer:getLevel() < moveList[i][4] then
			MoveCd:setText(moveList[i][4])
		end
		updateCooldown(MoveProgress, MoveCd, moveList[i][2], moveList[i][2], true)
	end
end

function updateCooldown(progress, label, duration, timer, init)
	if init and duration <= 0 then
		return progress:setPercent(100) and label:setText("")
	end

	progress:setPercent(100 - (timer/duration) * 100)

	if progress:getPercent() < 100 then
		label:setText(math.floor(timer/1000)+1)
		removeEvent(progress.event)
		progress.event = scheduleEvent(function() 
			updateCooldown(progress, label, duration, timer - 100)
		end, 100)
	else
		progress:setPercent(100)
		label:setText("")
	end
end
