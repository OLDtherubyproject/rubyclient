fishingButton = nil

function init()
  g_keyboard.bindKeyDown('Ctrl+Z', toggle)

  fishingButton = modules.client_topmenu.addRightGameToggleButton('fishingButton', tr('Fishing Rod') .. ' (Ctrl+Z)', '/images/topbuttons/fishing', toggle)
  fishingButton:setOn(false)
end

function terminate()
  g_keyboard.unbindKeyDown('Ctrl+Z')

  fishingButton:destroy()
end

function toggle()
  modules.game_interface.startUseWith(g_game.getLocalPlayer():getInventoryItem(InventorySlotHead))
end