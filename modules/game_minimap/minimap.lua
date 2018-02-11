minimapWidget = nil
minimapButton = nil
minimapWindow = nil
otmm = true
preloaded = false
fullmapView = false
oldZoom = nil
oldPos = nil
cityc = nil
tempoteleport = 0

function init()
  minimapButton = modules.client_topmenu.addRightGameToggleButton('minimapButton', tr('Minimap') .. ' (Ctrl+M)', '/images/topbuttons/minimap', toggle)
  minimapButton:setOn(true)
  minimapButton:setVisible(false)

  minimapWindow = g_ui.loadUI('minimap', modules.game_interface.getRightPanel())
  minimapWindow:setContentMinimumHeight(64)
  tpWindow = g_ui.displayUI('teleport')
  tpWindow:hide()

  minimapWidget = minimapWindow:recursiveGetChildById('minimap')

  local gameRootPanel = modules.game_interface.getRootPanel()
  g_keyboard.bindKeyPress('Alt+Left', function() minimapWidget:move(1,0) end, gameRootPanel)
  g_keyboard.bindKeyPress('Alt+Right', function() minimapWidget:move(-1,0) end, gameRootPanel)
  g_keyboard.bindKeyPress('Alt+Up', function() minimapWidget:move(0,1) end, gameRootPanel)
  g_keyboard.bindKeyPress('Alt+Down', function() minimapWidget:move(0,-1) end, gameRootPanel)
  g_keyboard.bindKeyDown('Ctrl+M', toggle)
  g_keyboard.bindKeyDown('Ctrl+Tab', toggleFullMap)

  minimapWindow:setup()

  connect(g_game, {
    onGameStart = online,
    onGameEnd = offline,
  })

  connect(LocalPlayer, {
    onPositionChange = updateCameraPosition
  })

  if g_game.isOnline() then
    online()
  end
--  ProtocolGame.registerExtendedOpcode(61, function(protocol, opcode, buffer) toggleFullMap(tonumber(buffer)) end)
  ProtocolGame.registerExtendedOpcode(62, function(protocol, opcode, buffer) if tonumber(buffer) then tempoteleport = tonumber(buffer) end toggleFullMap(buffer) end)
end

function terminate()
  if g_game.isOnline() then
    saveMap()
  end

  disconnect(g_game, {
    onGameStart = online,
    onGameEnd = offline,
  })

  disconnect(LocalPlayer, {
    onPositionChange = updateCameraPosition
  })

  local gameRootPanel = modules.game_interface.getRootPanel()
  g_keyboard.unbindKeyPress('Alt+Left', gameRootPanel)
  g_keyboard.unbindKeyPress('Alt+Right', gameRootPanel)
  g_keyboard.unbindKeyPress('Alt+Up', gameRootPanel)
  g_keyboard.unbindKeyPress('Alt+Down', gameRootPanel)
  g_keyboard.unbindKeyDown('Ctrl+M')
  g_keyboard.unbindKeyDown('Ctrl+Tab')

  minimapWindow:destroy()
  minimapButton:destroy()
  ProtocolGame.unregisterExtendedOpcode(61)
--  ProtocolGame.unregisterExtendedOpcode(61)
end

function toggle()
  if minimapButton:isOn() then
    minimapWindow:close()
    minimapButton:setOn(false)
  else
    minimapWindow:open()
    minimapButton:setOn(true)
  end
end

function onMiniWindowClose()
  minimapButton:setOn(false)
end

function preload()
  loadMap(false)
  preloaded = true
end

function online()
  loadMap(not preloaded)
  updateCameraPosition()
  minimapButton:setVisible(true)
end

function offline()
  saveMap()
  minimapButton:setVisible(false)
end

function loadMap(clean)
  local protocolVersion = g_game.getProtocolVersion()

  if clean then
    g_minimap.clean()
  end

  if otmm then
    local minimapFile = '/minimap.otmm'
    if g_resources.fileExists(minimapFile) then
      g_minimap.loadOtmm(minimapFile)
    end
  else
    local minimapFile = '/minimap_' .. protocolVersion .. '.otcm'
    if g_resources.fileExists(minimapFile) then
      g_map.loadOtcm(minimapFile)
    end
  end
  minimapWidget:load()
end

function saveMap()
  local protocolVersion = g_game.getProtocolVersion()
  if otmm then
    local minimapFile = '/minimap.otmm'
    g_minimap.saveOtmm(minimapFile)
  else
    local minimapFile = '/minimap_' .. protocolVersion .. '.otcm'
    g_map.saveOtcm(minimapFile)
  end
  minimapWidget:save()
end

local set = 0
local citys = { -- number = {x = 2561, y = 2520, z = 7}
{ name = "aeladon",    pos = {x = 2560, y = 2520, z = 7}, c = "celadon"},

--{ name = "Victory Road",  pos = { x = 1293, y = 1331, z = 7 } },  
}

function updateCameraPosition()
  local player = g_game.getLocalPlayer()
  if not player then return end
  local pos = player:getPosition()
  if not pos then return end
if set == 0 then
	
		if not minimapWidget:recursiveGetChildById('mapId') then
			local map = g_ui.createWidget('MinimapMain', minimapWidget)
			map:setImageSource('/images/ui/premap')
			map:setId('mapId')
			map:setOpacity(0.1)
			minimapWidget:centerInPosition(map, {x = 2447, y = 2459, z = 7})
		end
		
		local mapMain = minimapWidget:recursiveGetChildById('mapId')
		
		for i = 1, #citys do	
			local map = g_ui.createWidget('MinimapText', minimapWidget)
			minimapWidget:centerInPosition(map, citys[i].pos)
			map:setImageSource('/images/ui/citysName/'..citys[i].name)
			--map:setOpacity(1.0)
			map:setId('mapId' .. i)
			if citys[i].c then
			map.onClick = function() setTeleport(citys[i].name, citys[i].c)	end
			end
			map:setVisible(false)
		end
		
		
		local buttons = {"MinimapFloorUpButton", "MinimapFloorDownButton", "MinimapZoomInButton", "MinimapZoomOutButton", "MinimapResetButton", "TPLabel", "TPLabelText", "HouseTeleportButton"}
		
		for i = 1, #buttons do
			local map = g_ui.createWidget(buttons[i], minimapWidget)
		end
		
		minimapWidget:getChildById('housetp').onClick = function() setTeleport("your House", "house")	end
		set = 1
end
	if not minimapWidget:isDragging() then
		if not fullmapView then
			minimapWidget:setCameraPosition(player:getPosition())
		end
		minimapWidget:setCrossPosition(player:getPosition())
	end
	if not fullmapView then
		for i = 1, #citys do
			local map = minimapWidget:recursiveGetChildById('mapId' .. i)
			map:setVisible(false)
		end
		minimapWidget:getChildById('tptext'):setText("")
		minimapWidget:getChildById('tptime'):setText("")
	end
	
	if minimapWidget:getCameraPosition().z ~= 7 then
		local map = minimapWidget:recursiveGetChildById('mapId')
		map:setVisible(false)
		minimapWidget:setColor('black')
	end
end

function getFullMapView()
	return fullmapView and 1 or 0
end

function turnOnMyClock(tempo)
	if minimapWidget:getChildById('tptime'):isVisible() then
		if fullmapView then
			if tempo > 0 then
				minimapWidget:getChildById('tptime'):setText(getStringmytempo(tempo, "minutes")..".")
				marilene = scheduleEvent(function() turnOnMyClock(tempo-1) end,1000)
				minimapWidget:getChildById('tptime'):setMarginRight(-6)
			else
				minimapWidget:getChildById('tptime'):setText("Already.")
				minimapWidget:getChildById('tptime'):setMarginRight(4)
			end
		else
			minimapWidget:getChildById('tptext'):setText("")
			minimapWidget:getChildById('tptime'):setText("")
		end
	end
	return true
end

function toggleFullMap(opcode)
    minimapWidget:reset()
	 local zoom = oldZoom or 0
	  local pos = oldPos or minimapWidget:getCameraPosition()
	  oldZoom = minimapWidget:getZoom()
	  oldPos = minimapWidget:getCameraPosition()
	  minimapWidget:setZoom(zoom)
	  minimapWidget:setCameraPosition(pos)
  if not fullmapView and (not opcode or opcode ~= "yes") then
    fullmapView = true
    minimapWidget:setColor('#000000AA')
    minimapWindow:hide()
    minimapWidget:setParent(modules.game_interface.getRootPanel())
    minimapWidget:fill('parent')
    minimapWidget:setOpacity(0.9)
    minimapWidget:setAlternativeWidgetsVisible(true)
	local pos = minimapWidget:getCameraPosition()
	pos.z = 7
	minimapWidget:setCameraPosition(pos)
	if set == 1 then
		for i = 1, #citys do
			local map = minimapWidget:recursiveGetChildById('mapId' .. i)
			if map then map:setVisible(true) end
		end
	end
	minimapWidget:getChildById('tptext'):setText("Teleport time: ")
	if opcode and tonumber(opcode) > 0 then
	turnOnMyClock(tonumber(opcode))
	else
	minimapWidget:getChildById('tptime'):setText("Already.")
	minimapWidget:getChildById('tptime'):setMarginRight(4)
	end
  else
    fullmapView = false
    minimapWidget:setParent(minimapWindow:getChildById('contentsPanel'))
    minimapWidget:fill('parent')
    minimapWindow:show()
	minimapWidget:setOpacity(1.0)
    minimapWidget:setColor('#1F007A')
    minimapWidget:setAlternativeWidgetsVisible(false)
	if set == 1 then
		for i = 1, #citys do
			local map = minimapWidget:recursiveGetChildById('mapId' .. i)
			if map then map:setVisible(false) end
		end
	end
	minimapWidget:getChildById('tptext'):setText("")
	minimapWidget:getChildById('tptime'):setText("")
	removeEvent(marilene)
	hidetpWindow()
  end
  local map = minimapWidget:recursiveGetChildById('mapId')
	if minimapWidget:getCameraPosition().z ~= 7 then
		map:setVisible(false)
		minimapWidget:setColor('black')
		minimapWidget:setOpacity(0.9)
	else
		map:setVisible(true)
	end
end

function doHornytptime()
if horny1 ~= nil then removeEvent(horny1) end
if horny2 ~= nil then removeEvent(horny2) end
for a = 0,2 do
horny1 = scheduleEvent(function() minimapWidget:getChildById('tptime'):setColor('red') end,420*a)
end
for b = 0,2 do
horny2 = scheduleEvent(function() minimapWidget:getChildById('tptime'):setColor('white') end,210+(420*b))
end
return true
end

function setTeleport(name, c)
if tempoteleport > 0 then doHornytptime() return true end
cityc = c
local msg = "Are you sure you want to teleport to "..name.." ?"
tpWindow:show()
tpWindow:raise()
tpWindow:focus()
tpWindow:setText("Teleport")
tpWindow:getChildById('teleportText'):setText(msg)
return true
end

function doTeleport()
g_game.talk('h"'..cityc..',1')
tpWindow:hide()
return true
end

function hidetpWindow()
tpWindow:hide()
end

function getmytempo(number)
local s = number
local m = 0
local h = 0
	for a = 1, math.floor(number/60) do
	if s <= 59 then
	return {h = h, m = m, s = s}
	else
	s = s - 60
	m = m + 1
		if m >= 60 then
		m = 0
		h = h + 1
		end
	end
	end
	return {h = h, m = m, s = s}
end

function getStringmytempo(number, only)
local hour = ""
local minu = ""
local seco = ""
local Ntable = getmytempo(number)
	if Ntable.h <= 9 then
	hour = "0" .. Ntable.h .. ""
	else
	hour = ""..Ntable.h..""
	end
	if Ntable.m <= 9 then
	minu = "0"..Ntable.m..""
	else
	minu = ""..Ntable.m..""
	end
 	if Ntable.s <= 9 then
	seco = "0"..Ntable.s..""
	else
	seco = ""..Ntable.s..""
	end
	
if only and only == "minutes" and tonumber(hour) <= 0 then
	return minu..":"..seco..""
end

return ""..hour..":"..minu..":"..seco..""
end
