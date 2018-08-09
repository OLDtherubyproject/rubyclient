pokedexButton = nil

function init()
  g_keyboard.bindKeyDown('Ctrl+D', toggle)

  pokedexButton = modules.client_topmenu.addRightGameToggleButton('pokedexButton', tr('Pokedex') .. ' (Ctrl+D)', '/images/topbuttons/pokedex', toggle)
  pokedexButton:setOn(false)
end

function terminate()
  g_keyboard.unbindKeyDown('Ctrl+D')

  pokedexButton:destroy()
end

function toggle()
  modules.game_interface.startUseWith(g_game.getLocalPlayer():getInventoryItem(InventorySlotNecklance))
end