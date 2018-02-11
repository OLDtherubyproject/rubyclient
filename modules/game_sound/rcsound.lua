SOUNDS_CONFIG = {
	soundChannel = SoundChannels.Music,
	checkInterval = 500,
	folder = 'music/',
	noSound = 'Nenhum arquivo de som para esta area',
}

SOUNDS = {
	-- Skumdush Boeiros
	{fromPos = {x = 985, y = 1069, z = 8}, toPos = {x = 1378, y = 1290, z = 8}, sound = "Esgoto.ogg"},
	{fromPos = {x = 1165, y = 1120, z = 9}, toPos = {x = 1273, y = 1178, z = 9}, sound = "Esgoto.ogg"},
	
	-- Taverna --
	
		{fromPos = {x = 1165, y = 1223, z = 6}, toPos = {x = 1175, y = 1242, z = 6}, sound = "Taverna.ogg"},
	
	
	-- Navios --
	
		{fromPos = {x = 107, y = 359, z = 6}, toPos = {x = 110, y = 364, z = 6}, sound = "Fairy Tail - Main.ogg"},
		{fromPos = {x = 1145, y = 1217, z = 5}, toPos = {x = 1153, y = 1225, z = 5}, sound = "A Morgan.ogg"},
		{fromPos = {x = 1143, y = 1205, z = 7}, toPos = {x = 1153, y = 1225, z = 7}, sound = "A Morgan.ogg"},
	
		-- Boss
		{fromPos = {x=1172, y=1218, z=7}, toPos = {x=1178, y=1223, z=7}, priority = 1, sound="Fairy Tail - Main.ogg"},
		{fromPos = {x=1001, y=917, z=9}, toPos = {x=1002, y=919, z=9}, priority = 1, sound="FF VII - Main.ogg"},
	
	-- Main
	
		-- Dragon
		{fromPos = {x=1089, y=1110, z=7}, toPos = {x=1135, y=1142, z=7}, priority = 1, sound="Fairy Tail - Main.ogg"},
		{fromPos = {x=1102, y=1128, z=9}, toPos = {x=1131, y=1149, z=9}, priority = 1, sound="Fairy Tail - Main.ogg"},
		{fromPos = {x=1117, y=1124, z=9}, toPos = {x=1128, y=1127, z=9}, priority = 1, sound="Fairy Tail - Main.ogg"},
		{fromPos = {x=1100, y=1104, z=10}, toPos = {x=1127, y=1131, z=10}, priority = 1, sound="Fairy Tail - Main.ogg"},
		{fromPos = {x=1103, y=1108, z=9}, toPos = {x=1108, y=1112, z=9}, priority = 1, sound="Fairy Tail - Main.ogg"},

		{fromPos = {x=1096, y=1088, z=5}, toPos = {x=1110, y=1100, z=6}, priority = 1, sound="Fairy Tail - Main.ogg"},

		
		
} ----------

-- Sound
local rcSoundChannel
local showPosEvent
local playingSound

-- Design
soundWindow = nil
soundButton = nil

function toggle()
  if soundButton:isOn() then
    soundWindow:close()
    soundButton:setOn(false)
  else
    soundWindow:open()
    soundButton:setOn(true)
  end
end

function onMiniWindowClose()
  soundButton:setOn(false)
end

function init()
	for i = 1, #SOUNDS do
		SOUNDS[i].sound = SOUNDS_CONFIG.folder .. SOUNDS[i].sound
	end
	
	connect(g_game, { onGameStart = onGameStart,
                    onGameEnd = onGameEnd })
	
	rcSoundChannel = g_sounds.getChannel(SOUNDS_CONFIG.soundChannel)
	-- rcSoundChannel:setGain(value/COUNDS_CONFIG.volume)

	soundButton = modules.client_topmenu.addRightGameToggleButton('soundButton', tr('Sound Info') .. '', '/images/audio', toggle)
	soundButton:setOn(true)
	
	soundWindow = g_ui.loadUI('rcsound', modules.game_interface.getRightPanel())
	soundWindow:disableResize()
	soundWindow:setup()
	
	if(g_game.isOnline()) then
		onGameStart()
	end
end

function terminate()
	disconnect(g_game, { onGameStart = onGameStart,
                       onGameEnd = onGameEnd })
	onGameEnd()
	soundWindow:destroy()
	soundButton:destroy()
end

function onGameStart()
	stopSound()
	toggleSoundEvent = addEvent(toggleSound, SOUNDS_CONFIG.checkInterval)
end

function onGameEnd()
	stopSound()
	removeEvent(toggleSoundEvent)
end

function isInPos(pos, fromPos, toPos)
	return
		pos.x>=fromPos.x and
		pos.y>=fromPos.y and
		pos.z>=fromPos.z and
		pos.x<=toPos.x and
		pos.y<=toPos.y and
		pos.z<=toPos.z
end

function toggleSound()
	local player = g_game.getLocalPlayer()
	if not player then return end
	
	local pos = player:getPosition()
	local toPlay = nil

	for i = 1, #SOUNDS do
		if(isInPos(pos, SOUNDS[i].fromPos, SOUNDS[i].toPos)) then
			if(toPlay) then
				toPlay.priority = toPlay.priority or 0
				if((toPlay.sound~=SOUNDS[i].sound) and (SOUNDS[i].priority>toPlay.priority)) then
					toPlay = SOUNDS[i]
				end
			else
				toPlay = SOUNDS[i]
			end
		end
	end

	playingSound = playingSound or {sound='', priority=0}
	
	if(toPlay~=nil and playingSound.sound~=toPlay.sound) then
		g_logger.info("RC Sounds: New sound area detected:")
		g_logger.info("  Position: {x=" .. pos.x .. ", y=" .. pos.y .. ", z=" .. pos.z .. "}")
		g_logger.info("  Music: " .. toPlay.sound)
		stopSound()
		playSound(toPlay.sound)
		playingSound = toPlay
	elseif(toPlay==nil) and (playingSound.sound~='') then
		g_logger.info("RC Sounds: New sound area detected:")
		g_logger.info("  Left music area.")
		stopSound()
	end

	toggleSoundEvent = scheduleEvent(toggleSound, SOUNDS_CONFIG.checkInterval)
end

function playSound(sound)
	rcSoundChannel:enqueue(sound, 0)
	setLabel(clearName(sound))
end

function clearName(soundName)
	local explode = string.explode(soundName, "/")
	soundName = explode[#explode]
	explode = string.explode(soundName, ".ogg")
	soundName = ''
	for i = 1, #explode-1 do
		soundName = soundName .. explode[i]
	end
	return soundName
end

function stopSound()
	setLabel(SOUNDS_CONFIG.noSound)
	rcSoundChannel:stop()
	playingSound = nil
end

function setLabel(str)
	soundWindow:recursiveGetChildById('currentSound'):getChildById('value'):setText(str)
end