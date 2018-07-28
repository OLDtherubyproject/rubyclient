battleWindow = nil
battleButton = nil
battlePanel = nil
lastBattleButtonSwitched = nil
battleButtonsByCreaturesList = {}

mouseWidget = nil
pokemon = nil
pokemonId = nil
hidePlayersButton = nil
hideNPCsButton = nil
hidePokemonButton = nil

function init()
  g_ui.importStyle('battlebutton')
  battleButton = modules.client_topmenu.addRightGameToggleButton('battleButton', tr('Battle') .. ' (Ctrl+B)', '/images/topbuttons/battle', toggle)
  battleButton:setWidth(24)
  battleButton:setOn(true)
  battleWindow = g_ui.loadUI('battle', modules.game_interface.getRightPanel())
  g_keyboard.bindKeyDown('Ctrl+B', toggle)

  -- this disables scrollbar auto hiding
  local scrollbar = battleWindow:getChildById('miniwindowScrollBar')
  scrollbar:mergeStyle({ ['$!on'] = { }})

  battlePanel = battleWindow:recursiveGetChildById('battlePanel')

  hidePlayersButton = battleWindow:recursiveGetChildById('hidePlayers')
  hideNPCsButton = battleWindow:recursiveGetChildById('hideNPCs')
  hidePokemonButton = battleWindow:recursiveGetChildById('hidePokemon')

  mouseWidget = g_ui.createWidget('UIButton')
  mouseWidget:setVisible(false)
  mouseWidget:setFocusable(false)
  mouseWidget.cancelNextRelease = false

  battleWindow:setContentMinimumHeight(78)

  connect(Creature, {
    onGenderChange = updateCreatureGender,
    onEmblemChange = updateCreatureEmblem,
    onOutfitChange = onCreatureOutfitChange,
    onHealthPercentChange = onCreatureHealthPercentChange,
    onPositionChange = onCreaturePositionChange,
    onAppear = onCreatureAppear,
    onDisappear = onCreatureDisappear
  })

  connect(g_game, {
    onAttackingCreatureChange = onAttack,
    onFollowingCreatureChange = onFollow,
    onGameEnd = removeAllCreatures
  })

  ProtocolGame.registerExtendedOpcode(106, function(protocol, opcode, buffer) isMyPokemon(protocol, opcode, buffer) end)

  checkCreatures()
  battleWindow:setup()
end

function terminate()
  g_keyboard.unbindKeyDown('Ctrl+B')
  battleButtonsByCreaturesList = {}
  battleButton:destroy()
  battleWindow:destroy()
  mouseWidget:destroy()

  disconnect(Creature, {  
    onGenderChange = updateCreatureGender,
    onEmblemChange = updateCreatureEmblem,
    onOutfitChange = onCreatureOutfitChange,
    onHealthPercentChange = onCreatureHealthPercentChange,
    onPositionChange = onCreaturePositionChange,
    onAppear = onCreatureAppear,
    onDisappear = onCreatureDisappear
  })

  disconnect(g_game, {
    onAttackingCreatureChange = onAttack,
    onFollowingCreatureChange = onFollow,
    onGameEnd = removeAllCreatures
  })

  ProtocolGame.unregisterExtendedOpcode(106)
end

function toggle()
  if battleButton:isOn() then
    battleWindow:close()
    battleButton:setOn(false)
  else
    battleWindow:open()
    battleButton:setOn(true)
  end
end

function onMiniWindowClose()
  battleButton:setOn(false)
end

function checkCreatures()
  removeAllCreatures()

  local spectators = {}
  local player = g_game.getLocalPlayer()
  if g_game.isOnline() then
    --g_game.getProtocolGame():sendExtendedOpcode(106, 'refresh')
    creatures = g_map.getSpectators(player:getPosition(), false)
    for i, creature in ipairs(creatures) do
      if creature ~= player and doCreatureFitFilters(creature) then
        table.insert(spectators, creature)
      end
    end
  end

  for i, v in pairs(spectators) do
    addCreature(v)
  end
end

function isMyPokemon(protocol, opcode, buffer)
  local battleButton = battleButtonsByCreaturesList[tonumber(buffer)]
  local creature = battleButton:getCreature()
  if battleButton then
    removeCreature(creature)
  end
  pokemon = creature
  pokemonId = tonumber(buffer)
  addCreature(creature)
end

function doCreatureFitFilters(creature)
  local localPlayer = g_game.getLocalPlayer()
  if creature == localPlayer then
    return false
  end

  local pos = creature:getPosition()
  if not pos then return false end

  if pos.z ~= localPlayer:getPosition().z or not creature:canBeSeen() then return false end

  local hidePlayers = hidePlayersButton:isChecked()
  local hideNPCs = hideNPCsButton:isChecked()
  local hidePokemon = hidePokemonButton:isChecked()

  if hidePlayers and creature:isPlayer() then
    return false
  elseif hideNPCs and creature:isNpc() then
    return false
  elseif hidePokemon and creature:isPokemon() then
    return false
  end

  return true
end

function onCreatureHealthPercentChange(creature, health)
  local battleButton = battleButtonsByCreaturesList[creature:getId()]
  if battleButton then
    battleButton:setLifeBarPercent(creature:getHealthPercent())
	if creature:getHealthPercent() <= 0 then
	   battleButton:setVisible(false)
	end
  end
end

function onCreaturePositionChange(creature, newPos, oldPos)
  if creature:isLocalPlayer() then
    if oldPos and newPos and newPos.z ~= oldPos.z then
      checkCreatures()
    else
      for id, creatureButton in pairs(battleButtonsByCreaturesList) do
        addCreature(creatureButton.creature)
      end
    end
  else
    local has = hasCreature(creature)
    local fit = doCreatureFitFilters(creature)
    if has and not fit then
      removeCreature(creature)
    elseif fit then
      addCreature(creature)
    end
  end
end

function onCreatureOutfitChange(creature, outfit, oldOutfit)
  if doCreatureFitFilters(creature) then
    addCreature(creature)
  else
    removeCreature(creature)
  end
end

function onCreatureAppear(creature)
  if doCreatureFitFilters(creature) then
    addCreature(creature)
  end
end

function onCreatureDisappear(creature)
  removeCreature(creature)
end

function hasCreature(creature)
  return battleButtonsByCreaturesList[creature:getId()] ~= nil
end

function addCreature(creature)
  local creatureId = creature:getId()
  local battleButton = battleButtonsByCreaturesList[creatureId]
  
		if creature:getHealthPercent() <= 1 then
		   battleButton:setVisible(false)
		   return true
		end

  if not battleButton then
    battleButton = g_ui.createWidget('BattleButton')
    battleButton:setup(creature)

    battleButton.onHoverChange = onBattleButtonHoverChange
    battleButton.onMouseRelease = onBattleButtonMouseRelease

    battleButtonsByCreaturesList[creatureId] = battleButton

    if pokemonId and creatureId == pokemonId then
      battlePanel:insertChild(1, battleButton)
      battleButton:getChildById('myPokemon'):setImageColor('white')
    else
      battlePanel:addChild(battleButton)
    end

    if creature == g_game.getAttackingCreature() then
      onAttack(creature)
    end

    if creature == g_game.getFollowingCreature() then
      onFollow(creature)
    end
  else
    battleButton:setLifeBarPercent(creature:getHealthPercent())
  end

  local localPlayer = g_game.getLocalPlayer()
  battleButton:setVisible(localPlayer:hasSight(creature:getPosition()) and creature:canBeSeen())
end

function removeAllCreatures()
  for i, v in pairs(battleButtonsByCreaturesList) do
    removeCreature(v.creature)
  end
end

function removeCreature(creature)
  if hasCreature(creature) then
    local creatureId = creature:getId()

    if lastBattleButtonSwitched == battleButtonsByCreaturesList[creatureId] then
      lastBattleButtonSwitched = nil
    end

    battleButtonsByCreaturesList[creatureId].creature:hideStaticSquare()
    battleButtonsByCreaturesList[creatureId]:destroy()
    battleButtonsByCreaturesList[creatureId] = nil
  end
end

function onBattleButtonMouseRelease(self, mousePosition, mouseButton)
  if mouseWidget.cancelNextRelease then
    mouseWidget.cancelNextRelease = false
    return false
  end
  if ((g_mouse.isPressed(MouseLeftButton) and mouseButton == MouseRightButton) 
    or (g_mouse.isPressed(MouseRightButton) and mouseButton == MouseLeftButton)) then
    mouseWidget.cancelNextRelease = true
    g_game.look(self.creature)
    return true
  elseif mouseButton == MouseLeftButton and g_keyboard.isShiftPressed() then
    g_game.look(self.creature)
    return true
  elseif mouseButton == MouseRightButton and not g_mouse.isPressed(MouseLeftButton) then
    modules.game_interface.createThingMenu(mousePosition, nil, nil, self.creature)
    return true
  elseif mouseButton == MouseLeftButton and not g_mouse.isPressed(MouseRightButton) then
    if self.isTarget then
      g_game.cancelAttack()
    else
      g_game.attack(self.creature)
    end
    return true
  end
  return false
end

function onBattleButtonHoverChange(widget, hovered)
  if widget.isBattleButton then
    widget.isHovered = hovered
    updateBattleButton(widget)
  end
end

function onAttack(creature)
  local battleButton = creature and battleButtonsByCreaturesList[creature:getId()] or lastBattleButtonSwitched
  if battleButton then
    battleButton.isTarget = creature and true or false
    updateBattleButton(battleButton)
  end
end

function onFollow(creature)
  local battleButton = creature and battleButtonsByCreaturesList[creature:getId()] or lastBattleButtonSwitched
  if battleButton then
    battleButton.isFollowed = creature and true or false
    updateBattleButton(battleButton)
  end
end

function updateCreatureGender(creature, genderId)
  local battleButton = battleButtonsByCreaturesList[creature:getId()]
  if battleButton then
    battleButton:updateGender(genderId)
  end
end

function updateCreatureEmblem(creature, emblemId)
  local battleButton = battleButtonsByCreaturesList[creature:getId()]
  if battleButton then
    battleButton:updateGender(emblemId)
  end
end

function updateBattleButton(battleButton)
  battleButton:update()
  if battleButton.isTarget or battleButton.isFollowed then
    -- set new last battle button switched
    if lastBattleButtonSwitched and lastBattleButtonSwitched ~= battleButton then
      lastBattleButtonSwitched.isTarget = false
      lastBattleButtonSwitched.isFollowed = false
      updateBattleButton(lastBattleButtonSwitched)
    end
    lastBattleButtonSwitched = battleButton
  end
end

function getMyPokemon()
  if pokemon then
    return pokemon
  end
  return false
end
