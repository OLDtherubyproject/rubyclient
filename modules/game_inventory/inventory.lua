InventorySlotStyles = {
  [InventorySlotHead] = "HeadSlot",
  [InventorySlotNeck] = "NeckSlot",
  [InventorySlotBack] = "BackSlot",
  [InventorySlotBody] = "BodySlot",
  [InventorySlotRight] = "RightSlot",
  [InventorySlotLeft] = "LeftSlot",
  [InventorySlotLeg] = "LegSlot",
  [InventorySlotFeet] = "FeetSlot",
  [InventorySlotFinger] = "FingerSlot",
  [InventorySlotAmmo] = "AmmoSlot"
}

inventoryButton = nil

function init()
  g_keyboard.bindKeyDown('Ctrl+I', toggle)

  inventoryButton = modules.client_topmenu.addRightGameToggleButton('inventoryButton', tr('Inventory') .. ' (Ctrl+I)', '/images/topbuttons/inventory', toggle)
  inventoryButton:setOn(false)
end

function terminate()
  g_keyboard.unbindKeyDown('Ctrl+I')

  inventoryButton:destroy()
end

function toggle()
  if inventoryButton:isOn() then
    local backpack = g_game.getLocalPlayer():getInventoryItem(InventorySlotBack)
    if backpack and backpack:isContainer() then
      g_game.close(backpack, backpack:getParentContainer())
    end
  else
    local backpack = g_game.getLocalPlayer():getInventoryItem(InventorySlotBack)
    if backpack and backpack:isContainer() then
      g_game.open(backpack, backpack:getParentContainer())
    end    
  end
end