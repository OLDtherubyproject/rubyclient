function g_game.getRsa()
  return G.currentRsa
end

function g_game.findPlayerItem(itemId, subType)
    local localPlayer = g_game.getLocalPlayer()
    if localPlayer then
        for slot = InventorySlotFirst, InventorySlotLast do
            local item = localPlayer:getInventoryItem(slot)
            if item and item:getId() == itemId and (subType == -1 or item:getSubType() == subType) then
                return item
            end
        end
    end

    return g_game.findItemInContainers(itemId, subType)
end

function g_game.setRSAandOS()
  g_game.setRsa(RUBYSERV_RSA)
end

function g_game.setRsa(rsa, e)
  e = e or '65537'
  g_crypt.rsaSetPublicKey(rsa, e)
  G.currentRsa = rsa
end

function g_game.getSupportedClients()
  return {
    100
  }
end
-- The client version and protocol version where
-- unsynchronized for some releases, not sure if this
-- will be the normal standard.

-- Client Version: Publicly given version when
-- downloading Cipsoft client.

-- Protocol Version: Previously was the same as
-- the client version, but was unsychronized in some
-- releases, now it needs to be verified and added here
-- if it does not match the client version.

-- Reason for defining both: The server now requires a
-- Client version and Protocol version from the client.

-- Important: Use getClientVersion for specific protocol
-- features to ensure we are using the proper version.

function g_game.getClientProtocolVersion()
  return 100
end

if not G.currentRsa then
  g_game.setRsa(RUBYSERV_RSA)
end
