/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "protocolgame.h"

#include "localplayer.h"
#include "thingtypemanager.h"
#include "game.h"
#include "map.h"
#include "item.h"
#include "effect.h"
#include "missile.h"
#include "tile.h"
#include "luavaluecasts.h"
#include <framework/core/eventdispatcher.h>

void ProtocolGame::parseMessage(const InputMessagePtr& msg)
{
    uint8_t opcode = -1;
    uint8_t prevOpcode = -1;

    try {
        while(!msg->eof()) {
            opcode = msg->getByte();

            // must be > so extended will be enabled before GameStart.
            if(!g_game.getFeature(Otc::GameLoginPending)) {
                if(!m_gameInitialized && opcode > Proto::GameServerFirstGameOpcode) {
                    g_game.processGameStart();
                    m_gameInitialized = true;
                }
            }

            // try to parse in lua first
            int readPos = msg->getReadPos();
            if(callLuaField<bool>("onOpcode", opcode, msg))
                continue;
            else
                msg->setReadPos(readPos); // restore read pos

            switch(opcode) {
            case Proto::GameServerLoginOrPendingState:
                if(g_game.getFeature(Otc::GameLoginPending))
                    parsePendingGame(msg);
                else
                    parseLogin(msg);
                break;
            case Proto::GameServerGMActions:
                parseGMActions(msg);
                break;
            case Proto::GameServerUpdateNeeded:
                parseUpdateNeeded(msg);
                break;
            case Proto::GameServerLoginError:
                parseLoginError(msg);
                break;
            case Proto::GameServerLoginAdvice:
                parseLoginAdvice(msg);
                break;
            case Proto::GameServerLoginWait:
                parseLoginWait(msg);
                break;
            case Proto::GameServerLoginToken:
                parseLoginToken(msg);
                break;
            case Proto::GameServerPing:
            case Proto::GameServerPingBack:
                if((opcode == Proto::GameServerPing && g_game.getFeature(Otc::GameClientPing)) ||
                   (opcode == Proto::GameServerPingBack && !g_game.getFeature(Otc::GameClientPing)))
                    parsePingBack(msg);
                else
                    parsePing(msg);
                break;
            case Proto::GameServerChallenge:
                parseChallenge(msg);
                break;
            case Proto::GameServerDeath:
                parseDeath(msg);
                break;
            case Proto::GameServerFullMap:
                parseMapDescription(msg);
                break;
            case Proto::GameServerMapTopRow:
                parseMapMoveNorth(msg);
                break;
            case Proto::GameServerMapRightRow:
                parseMapMoveEast(msg);
                break;
            case Proto::GameServerMapBottomRow:
                parseMapMoveSouth(msg);
                break;
            case Proto::GameServerMapLeftRow:
                parseMapMoveWest(msg);
                break;
            case Proto::GameServerUpdateTile:
                parseUpdateTile(msg);
                break;
            case Proto::GameServerCreateOnMap:
                parseTileAddThing(msg);
                break;
            case Proto::GameServerChangeOnMap:
                parseTileTransformThing(msg);
                break;
            case Proto::GameServerDeleteOnMap:
                parseTileRemoveThing(msg);
                break;
            case Proto::GameServerMoveCreature:
                parseCreatureMove(msg);
                break;
            case Proto::GameServerOpenContainer:
                parseOpenContainer(msg);
                break;
            case Proto::GameServerCloseContainer:
                parseCloseContainer(msg);
                break;
            case Proto::GameServerCreateContainer:
                parseContainerAddItem(msg);
                break;
            case Proto::GameServerChangeInContainer:
                parseContainerUpdateItem(msg);
                break;
            case Proto::GameServerDeleteInContainer:
                parseContainerRemoveItem(msg);
                break;
            case Proto::GameServerSetInventory:
                parseAddInventoryItem(msg);
                break;
            case Proto::GameServerDeleteInventory:
                parseRemoveInventoryItem(msg);
                break;
            case Proto::GameServerOpenNpcTrade:
                parseOpenNpcTrade(msg);
                break;
            case Proto::GameServerPlayerGoods:
                parsePlayerGoods(msg);
                break;
            case Proto::GameServerCloseNpcTrade:
                parseCloseNpcTrade(msg);
                break;
            case Proto::GameServerOwnTrade:
                parseOwnTrade(msg);
                break;
            case Proto::GameServerCounterTrade:
                parseCounterTrade(msg);
                break;
            case Proto::GameServerCloseTrade:
                parseCloseTrade(msg);
                break;
            case Proto::GameServerAmbient:
                parseWorldLight(msg);
                break;
            case Proto::GameServerGraphicalEffect:
                parseMagicEffect(msg);
                break;
            case Proto::GameServerTextEffect:
                parseAnimatedText(msg);
                break;
            case Proto::GameServerMissleEffect:
                parseDistanceMissile(msg);
                break;
            case Proto::GameServerMarkCreature:
                parseCreatureMark(msg);
                break;
            case Proto::GameServerTrappers:
                parseTrappers(msg);
                break;
            case Proto::GameServerCreatureHealth:
                parseCreatureHealth(msg);
                break;
            case Proto::GameServerCreatureLight:
                parseCreatureLight(msg);
                break;
            case Proto::GameServerCreatureOutfit:
                parseCreatureOutfit(msg);
                break;
            case Proto::GameServerCreatureSpeed:
                parseCreatureSpeed(msg);
                break;
            case Proto::GameServerCreatureGender:
                parseCreatureGenders(msg);
                break;
            case Proto::GameServerCreatureParty:
                parseCreatureShields(msg);
                break;
            case Proto::GameServerCreatureUnpass:
                parseCreatureUnpass(msg);
                break;
            case Proto::GameServerEditText:
                parseEditText(msg);
                break;
            case Proto::GameServerEditList:
                parseEditList(msg);
                break;
            // PROTOCOL>=1038
            case Proto::GameServerPremiumTrigger:
                parsePremiumTrigger(msg);
                break;
            case Proto::GameServerPlayerData:
                parsePlayerStats(msg);
                break;
            case Proto::GameServerPlayerSkills:
                parsePlayerSkills(msg);
                break;
            case Proto::GameServerPlayerState:
                parsePlayerState(msg);
                break;
            case Proto::GameServerClearTarget:
                parsePlayerCancelAttack(msg);
                break;
            case Proto::GameServerPlayerModes:
                parsePlayerModes(msg);
                break;
            case Proto::GameServerTalk:
                parseTalk(msg);
                break;
            case Proto::GameServerChannels:
                parseChannelList(msg);
                break;
            case Proto::GameServerOpenChannel:
                parseOpenChannel(msg);
                break;
            case Proto::GameServerOpenPrivateChannel:
                parseOpenPrivateChannel(msg);
                break;
            case Proto::GameServerRuleViolationChannel:
                parseRuleViolationChannel(msg);
                break;
            case Proto::GameServerRuleViolationRemove:
                parseRuleViolationRemove(msg);
                break;
            case Proto::GameServerRuleViolationCancel:
                parseRuleViolationCancel(msg);
                break;
            case Proto::GameServerRuleViolationLock:
                parseRuleViolationLock(msg);
                break;
            case Proto::GameServerOpenOwnChannel:
                parseOpenOwnPrivateChannel(msg);
                break;
            case Proto::GameServerCloseChannel:
                parseCloseChannel(msg);
                break;
            case Proto::GameServerTextMessage:
                parseTextMessage(msg);
                break;
            case Proto::GameServerCancelWalk:
                parseCancelWalk(msg);
                break;
            case Proto::GameServerWalkWait:
                parseWalkWait(msg);
                break;
            case Proto::GameServerFloorChangeUp:
                parseFloorChangeUp(msg);
                break;
            case Proto::GameServerFloorChangeDown:
                parseFloorChangeDown(msg);
                break;
            case Proto::GameServerChooseOutfit:
                parseOpenOutfitWindow(msg);
                break;
            case Proto::GameServerVipAdd:
                parseVipAdd(msg);
                break;
            case Proto::GameServerVipState:
                parseVipState(msg);
                break;
            case Proto::GameServerVipLogout:
                parseVipLogout(msg);
                break;
            case Proto::GameServerTutorialHint:
                parseTutorialHint(msg);
                break;
            case Proto::GameServerAutomapFlag:
                parseAutomapFlag(msg);
                break;
            case Proto::GameServerQuestLog:
                parseQuestLog(msg);
                break;
            case Proto::GameServerQuestLine:
                parseQuestLine(msg);
                break;
            // PROTOCOL>=870
            case Proto::GameServerSpellDelay:
                parseSpellCooldown(msg);
                break;
            case Proto::GameServerSpellGroupDelay:
                parseSpellGroupCooldown(msg);
                break;
            case Proto::GameServerMultiUseDelay:
                parseMultiUseCooldown(msg);
                break;
            // PROTOCOL>=910
            case Proto::GameServerChannelEvent:
                parseChannelEvent(msg);
                break;
            case Proto::GameServerItemInfo:
                parseItemInfo(msg);
                break;
            case Proto::GameServerPlayerInventory:
                parsePlayerInventory(msg);
                break;
            // PROTOCOL>=950
            case Proto::GameServerPlayerDataBasic:
                parsePlayerInfo(msg);
                break;
            // PROTOCOL>=970
            case Proto::GameServerModalDialog:
                parseModalDialog(msg);
                break;
            // PROTOCOL>=980
            case Proto::GameServerLoginSuccess:
                parseLogin(msg);
                break;
            case Proto::GameServerEnterGame:
                parseEnterGame(msg);
                break;
            case Proto::GameServerPlayerHelpers:
                parsePlayerHelpers(msg);
                break;
            // PROTOCOL>=1000
            case Proto::GameServerCreatureMarks:
                parseCreaturesMark(msg);
                break;
            case Proto::GameServerCreatureType:
                parseCreatureType(msg);
                break;
            // PROTOCOL>=1055
            case Proto::GameServerBlessings:
                parseBlessings(msg);
                break;
            case Proto::GameServerUnjustifiedStats:
                parseUnjustifiedStats(msg);
                break;
            case Proto::GameServerPvpSituations:
                parsePvpSituations(msg);
                break;
            case Proto::GameServerPreset:
                parsePreset(msg);
                break;
            // PROTOCOL>=1080
            case Proto::GameServerCoinBalanceUpdating:
                parseCoinBalanceUpdating(msg);
                break;
            case Proto::GameServerCoinBalance:
                parseCoinBalance(msg);
                break;
            case Proto::GameServerRequestPurchaseData:
                parseRequestPurchaseData(msg);
                break;
            case Proto::GameServerStoreCompletePurchase:
                parseCompleteStorePurchase(msg);
                break;
            case Proto::GameServerStoreOffers:
                parseStoreOffers(msg);
                break;
            case Proto::GameServerStoreTransactionHistory:
                parseStoreTransactionHistory(msg);
                break;
            case Proto::GameServerStoreError:
                parseStoreError(msg);
                break;
            case Proto::GameServerStore:
                parseStore(msg);
                break;
            // PROTOCOL>=1097
            case Proto::GameServerStoreButtonIndicators:
                parseStoreButtonIndicators(msg);
                break;
            case Proto::GameServerSetStoreDeepLink:
                parseSetStoreDeepLink(msg);
                break;
            // otclient ONLY
            case Proto::GameServerExtendedOpcode:
                parseExtendedOpcode(msg);
                break;
            case Proto::GameServerChangeMapAwareRange:
                parseChangeMapAwareRange(msg);
                break;
            default:
                stdext::throw_exception(stdext::format("unhandled opcode %d", (int)opcode));
                break;
            }
            prevOpcode = opcode;
        }
    } catch(stdext::exception& e) {
        g_logger.error(stdext::format("ProtocolGame parse message exception (%d bytes unread, last opcode is %d, prev opcode is %d): %s",
                                      msg->getUnreadSize(), opcode, prevOpcode, e.what()));
    }
}

void ProtocolGame::parseLogin(const InputMessagePtr& msg)
{
    unsigned int playerId = msg->get<uint32_t>();
    int serverBeat = msg->get<uint16_t>();

    if(g_game.getFeature(Otc::GameNewSpeedLaw)) {
        double speedA = msg->getDouble();
        double speedB = msg->getDouble();
        double speedC = msg->getDouble();
        m_localPlayer->setSpeedFormula(speedA, speedB, speedC);
    }
    bool canReportBugs = msg->getByte();

    msg->getByte(); // can change pvp frame option

    int expertModeEnabled = msg->getByte();
    g_game.setExpertPvpMode(expertModeEnabled);

    if(g_game.getFeature(Otc::GameIngameStore)) {
        // URL to ingame store images
        msg->getString();

        // premium coin package size
        // e.g you can only buy packs of 25, 50, 75, .. coins in the market
        msg->get<uint16_t>();
    }

    m_localPlayer->setId(playerId);
    g_game.setServerBeat(serverBeat);
    g_game.setCanReportBugs(canReportBugs);

    g_game.processLogin();
}

void ProtocolGame::parsePendingGame(const InputMessagePtr& msg)
{
    //set player to pending game state
    g_game.processPendingGame();
}

void ProtocolGame::parseEnterGame(const InputMessagePtr& msg)
{
    //set player to entered game state
    g_game.processEnterGame();

    if(!m_gameInitialized) {
        g_game.processGameStart();
        m_gameInitialized = true;
    }
}

void ProtocolGame::parseStoreButtonIndicators(const InputMessagePtr& msg)
{
    msg->getByte(); // unknown
    msg->getByte(); // unknown
}

void ProtocolGame::parseSetStoreDeepLink(const InputMessagePtr& msg)
{
    int currentlyFeaturedServiceType = msg->getByte();
}

void ProtocolGame::parseBlessings(const InputMessagePtr& msg)
{
    uint16_t blessings = msg->get<uint16_t>();
    m_localPlayer->setBlessings(blessings);
}

void ProtocolGame::parsePreset(const InputMessagePtr& msg)
{
    uint32_t preset = msg->get<uint32_t>();
}

void ProtocolGame::parseRequestPurchaseData(const InputMessagePtr& msg)
{
    int transactionId = msg->get<uint32_t>();
    int productType = msg->getByte();
}

void ProtocolGame::parseStore(const InputMessagePtr& msg)
{
    parseCoinBalance(msg);

    // Parse all categories
    int count = msg->get<uint16_t>();
    for(int i = 0; i < count; i++) {
        std::string category = msg->getString();
        std::string description = msg->getString();

        int highlightState = 0;
        if(g_game.getFeature(Otc::GameIngameStoreHighlights))
            highlightState = msg->getByte();

        std::vector<std::string> icons;
        int iconCount = msg->getByte();
        for(int i = 0; i < iconCount; i++) {
            std::string icon = msg->getString();
            icons.push_back(icon);
        }

        // If this is a valid category name then
        // the category we just parsed is a child of that
        std::string parentCategory = msg->getString();
    }
}

void ProtocolGame::parseCoinBalance(const InputMessagePtr& msg)
{
    bool update = msg->getByte() == 1;
    int coins = -1;
    int transferableCoins = -1;
    if(update) {
        // amount of coins that can be used to buy prodcuts
        // in the ingame store
        coins = msg->get<uint32_t>();

        // amount of coins that can be sold in market
        // or be transfered to another player
        transferableCoins = msg->get<uint32_t>();
    }
}

void ProtocolGame::parseCoinBalanceUpdating(const InputMessagePtr& msg)
{
    // coin balance can be updating and might not be accurate
    bool isUpdating = msg->getByte() == 1;
}

void ProtocolGame::parseCompleteStorePurchase(const InputMessagePtr& msg)
{
    // not used
    msg->getByte();

    std::string message = msg->getString();
    int coins = msg->get<uint32_t>();
    int transferableCoins = msg->get<uint32_t>();

    g_logger.info(stdext::format("Purchase Complete: %s", message));
}

void ProtocolGame::parseStoreTransactionHistory(const InputMessagePtr &msg)
{
    int currentPage = msg->get<uint32_t>();
    int pageCount = msg->get<uint32_t>();

    int entries = msg->getByte();
    for(int i = 0; i < entries; i++) {
        int time = msg->get<uint16_t>();
        int productType = msg->getByte();
        int coinChange = msg->get<uint32_t>();
        std::string productName = msg->getString();
        g_logger.error(stdext::format("Time %i, type %i, change %i, product name %s", time, productType, coinChange, productName));
    }
}

void ProtocolGame::parseStoreOffers(const InputMessagePtr& msg)
{
    std::string categoryName = msg->getString();

    int offers = msg->get<uint16_t>();
    for(int i = 0; i < offers; i++) {
        int offerId = msg->get<uint32_t>();
        std::string offerName = msg->getString();
        std::string offerDescription = msg->getString();

        int price = msg->get<uint32_t>();
        int highlightState = msg->getByte();
        if(highlightState == 2 && g_game.getFeature(Otc::GameIngameStoreHighlights)) {
            int saleValidUntilTimestamp = msg->get<uint32_t>();
            int basePrice = msg->get<uint32_t>();
        }

        int disabledState = msg->getByte();
        std::string disabledReason = "";
        if(g_game.getFeature(Otc::GameIngameStoreHighlights) && disabledState == 1) {
            disabledReason = msg->getString();
        }

        int icons = msg->getByte();
        for(int j = 0; j < icons; j++) {
            std::string icon = msg->getString();
        }

        int subOffers = msg->get<uint16_t>();
        for(int j = 0; j < subOffers; j++) {
            std::string name = msg->getString();
            std::string description = msg->getString();

            int subIcons = msg->getByte();
            for(int k = 0; k < subIcons; k++) {
                std::string icon = msg->getString();
            }
            std::string serviceType = msg->getString();
        }
    }
}

void ProtocolGame::parseStoreError(const InputMessagePtr& msg)
{
    int errorType = msg->getByte();
    std::string message = msg->getString();
    g_logger.error(stdext::format("Store Error: %s [%i]", message, errorType));
}

void ProtocolGame::parseUnjustifiedStats(const InputMessagePtr& msg)
{
    UnjustifiedPoints unjustifiedPoints;
    unjustifiedPoints.killsDay = msg->getByte();
    unjustifiedPoints.killsDayRemaining = msg->getByte();
    unjustifiedPoints.killsWeek = msg->getByte();
    unjustifiedPoints.killsWeekRemaining = msg->getByte();
    unjustifiedPoints.killsMonth = msg->getByte();
    unjustifiedPoints.killsMonthRemaining = msg->getByte();
    unjustifiedPoints.genderTime = msg->getByte();

    g_game.setUnjustifiedPoints(unjustifiedPoints);
}

void ProtocolGame::parsePvpSituations(const InputMessagePtr& msg)
{
    uint8_t openPvpSituations = msg->getByte();

    g_game.setOpenPvpSituations(openPvpSituations);
}

void ProtocolGame::parsePlayerHelpers(const InputMessagePtr& msg)
{
    unsigned int id = msg->get<uint32_t>();
    int helpers = msg->get<uint16_t>();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        g_game.processPlayerHelpers(helpers);
    else
        g_logger.traceError(stdext::format("could not get creature with id %d", id));
}

void ProtocolGame::parseGMActions(const InputMessagePtr& msg)
{
    std::vector<uint8_t> actions;

    int numViolationReasons;

    numViolationReasons = 20;

    for(int i = 0; i < numViolationReasons; ++i)
        actions.push_back(msg->getByte());
    g_game.processGMActions(actions);
}

void ProtocolGame::parseUpdateNeeded(const InputMessagePtr& msg)
{
    std::string signature = msg->getString();
    g_game.processUpdateNeeded(signature);
}

void ProtocolGame::parseLoginError(const InputMessagePtr& msg)
{
    std::string error = msg->getString();

    g_game.processLoginError(error);
}

void ProtocolGame::parseLoginAdvice(const InputMessagePtr& msg)
{
    std::string message = msg->getString();

    g_game.processLoginAdvice(message);
}

void ProtocolGame::parseLoginWait(const InputMessagePtr& msg)
{
    std::string message = msg->getString();
    int time = msg->getByte();

    g_game.processLoginWait(message, time);
}

void ProtocolGame::parseLoginToken(const InputMessagePtr& msg)
{
    bool unknown = (msg->getByte() == 0);
    g_game.processLoginToken(unknown);
}

void ProtocolGame::parsePing(const InputMessagePtr& msg)
{
    g_game.processPing();
}

void ProtocolGame::parsePingBack(const InputMessagePtr& msg)
{
    g_game.processPingBack();
}

void ProtocolGame::parseChallenge(const InputMessagePtr& msg)
{
    unsigned int timestamp = msg->get<uint32_t>();
    uint8_t random = msg->getByte();

    sendLoginPacket(timestamp, random);
}

void ProtocolGame::parseDeath(const InputMessagePtr& msg)
{
    int penality = 100;
    int deathType = Otc::DeathRegular;

    if(g_game.getFeature(Otc::GameDeathType))
        deathType = msg->getByte();

    if(g_game.getFeature(Otc::GamePenalityOnDeath) && deathType == Otc::DeathRegular)
        penality = msg->getByte();

    g_game.processDeath(deathType, penality);
}

void ProtocolGame::parseMapDescription(const InputMessagePtr& msg)
{
    Position pos = getPosition(msg);

    if(!m_mapKnown)
        m_localPlayer->setPosition(pos);

    g_map.setCentralPosition(pos);

    AwareRange range = g_map.getAwareRange();
    setMapDescription(msg, pos.x - range.left, pos.y - range.top, pos.z, range.horizontal(), range.vertical());

    if(!m_mapKnown) {
        g_dispatcher.addEvent([] { g_lua.callGlobalField("g_game", "onMapKnown"); });
        m_mapKnown = true;
    }

    g_dispatcher.addEvent([] { g_lua.callGlobalField("g_game", "onMapDescription"); });
}

void ProtocolGame::parseMapMoveNorth(const InputMessagePtr& msg)
{
    Position pos;
    if(g_game.getFeature(Otc::GameMapMovePosition))
        pos = getPosition(msg);
    else
        pos = g_map.getCentralPosition();
    pos.y--;

    AwareRange range = g_map.getAwareRange();
    setMapDescription(msg, pos.x - range.left, pos.y - range.top, pos.z, range.horizontal(), 1);
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseMapMoveEast(const InputMessagePtr& msg)
{
    Position pos;
    if(g_game.getFeature(Otc::GameMapMovePosition))
        pos = getPosition(msg);
    else
        pos = g_map.getCentralPosition();
    pos.x++;

    AwareRange range = g_map.getAwareRange();
    setMapDescription(msg, pos.x + range.right, pos.y - range.top, pos.z, 1, range.vertical());
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseMapMoveSouth(const InputMessagePtr& msg)
{
    Position pos;
    if(g_game.getFeature(Otc::GameMapMovePosition))
        pos = getPosition(msg);
    else
        pos = g_map.getCentralPosition();
    pos.y++;

    AwareRange range = g_map.getAwareRange();
    setMapDescription(msg, pos.x - range.left, pos.y + range.bottom, pos.z, range.horizontal(), 1);
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseMapMoveWest(const InputMessagePtr& msg)
{
    Position pos;
    if(g_game.getFeature(Otc::GameMapMovePosition))
        pos = getPosition(msg);
    else
        pos = g_map.getCentralPosition();
    pos.x--;

    AwareRange range = g_map.getAwareRange();
    setMapDescription(msg, pos.x - range.left, pos.y - range.top, pos.z, 1, range.vertical());
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseUpdateTile(const InputMessagePtr& msg)
{
    Position tilePos = getPosition(msg);
    setTileDescription(msg, tilePos);
}

void ProtocolGame::parseTileAddThing(const InputMessagePtr& msg)
{
    Position pos = getPosition(msg);
    int stackPos = msg->getByte();

    ThingPtr thing = getThing(msg);
    g_map.addThing(thing, pos, stackPos);
}

void ProtocolGame::parseTileTransformThing(const InputMessagePtr& msg)
{
    ThingPtr thing = getMappedThing(msg);
    ThingPtr newThing = getThing(msg);

    if(!thing) {
        g_logger.traceError("no thing");
        return;
    }

    Position pos = thing->getPosition();
    int stackpos = thing->getStackPos();

    if(!g_map.removeThing(thing)) {
        g_logger.traceError("unable to remove thing");
        return;
    }

    g_map.addThing(newThing, pos, stackpos);
}

void ProtocolGame::parseTileRemoveThing(const InputMessagePtr& msg)
{
    ThingPtr thing = getMappedThing(msg);
    if(!thing) {
        g_logger.traceError("no thing");
        return;
    }

    if(!g_map.removeThing(thing))
        g_logger.traceError("unable to remove thing");
}

void ProtocolGame::parseCreatureMove(const InputMessagePtr& msg)
{
    ThingPtr thing = getMappedThing(msg);
    Position newPos = getPosition(msg);

    if(!thing || !thing->isCreature()) {
        g_logger.traceError("no creature found to move");
        return;
    }

    if(!g_map.removeThing(thing)) {
        g_logger.traceError("unable to remove creature");
        return;
    }

    CreaturePtr creature = thing->static_self_cast<Creature>();
    creature->allowAppearWalk();

    g_map.addThing(thing, newPos, -1);
}

void ProtocolGame::parseOpenContainer(const InputMessagePtr& msg)
{
    int containerId = msg->getByte();
    ItemPtr containerItem = getItem(msg);
    std::string name = msg->getString();
    int capacity = msg->getByte();
    bool hasParent = (msg->getByte() != 0);

    bool isUnlocked = true;
    bool hasPages = false;
    int containerSize = 0;
    int firstIndex = 0;

    if(g_game.getFeature(Otc::GameContainerPagination)) {
        isUnlocked = (msg->getByte() != 0); // drag and drop
        hasPages = (msg->getByte() != 0); // pagination
        containerSize = msg->get<uint16_t>(); // container size
        firstIndex = msg->get<uint16_t>(); // first index
    }

    int itemCount = msg->getByte();

    std::vector<ItemPtr> items(itemCount);
    for(int i = 0; i < itemCount; i++)
        items[i] = getItem(msg);

    g_game.processOpenContainer(containerId, containerItem, name, capacity, hasParent, items, isUnlocked, hasPages, containerSize, firstIndex);
}

void ProtocolGame::parseCloseContainer(const InputMessagePtr& msg)
{
    int containerId = msg->getByte();
    g_game.processCloseContainer(containerId);
}

void ProtocolGame::parseContainerAddItem(const InputMessagePtr& msg)
{
    int containerId = msg->getByte();
    int slot = 0;
    if(g_game.getFeature(Otc::GameContainerPagination)) {
        slot = msg->get<uint16_t>(); // slot
    }
    ItemPtr item = getItem(msg);
    g_game.processContainerAddItem(containerId, item, slot);
}

void ProtocolGame::parseContainerUpdateItem(const InputMessagePtr& msg)
{
    int containerId = msg->getByte();
    int slot;
    if(g_game.getFeature(Otc::GameContainerPagination)) {
        slot = msg->get<uint16_t>();
    } else {
        slot = msg->getByte();
    }
    ItemPtr item = getItem(msg);
    g_game.processContainerUpdateItem(containerId, slot, item);
}

void ProtocolGame::parseContainerRemoveItem(const InputMessagePtr& msg)
{
    int containerId = msg->getByte();
    int slot;
    ItemPtr lastItem;
    if(g_game.getFeature(Otc::GameContainerPagination)) {
        slot = msg->get<uint16_t>();

        int itemId = msg->get<uint16_t>();
        if(itemId != 0)
            lastItem = getItem(msg, itemId);
    } else {
        slot = msg->getByte();
    }
    g_game.processContainerRemoveItem(containerId, slot, lastItem);
}

void ProtocolGame::parseAddInventoryItem(const InputMessagePtr& msg)
{
    int slot = msg->getByte();
    ItemPtr item = getItem(msg);
    g_game.processInventoryChange(slot, item);
}

void ProtocolGame::parseRemoveInventoryItem(const InputMessagePtr& msg)
{
    int slot = msg->getByte();
    g_game.processInventoryChange(slot, ItemPtr());
}

void ProtocolGame::parseOpenNpcTrade(const InputMessagePtr& msg)
{
    std::vector<std::tuple<ItemPtr, std::string, int, int, int>> items;
    std::string npcName;

    if(g_game.getFeature(Otc::GameNameOnNpcTrade))
        npcName = msg->getString();

    int listCount = msg->get<uint16_t>();

    for(int i = 0; i < listCount; ++i) {
        uint16_t itemId = msg->get<uint16_t>();
        uint16_t count = msg->get<uint16_t>();

        ItemPtr item = Item::create(itemId);
        item->setCountOrSubType(count);

        std::string name = msg->getString();
        int weight = msg->get<uint32_t>();
        int buyPrice = msg->get<uint32_t>();
        int sellPrice = msg->get<uint32_t>();
        items.push_back(std::make_tuple(item, name, weight, buyPrice, sellPrice));
    }

    g_game.processOpenNpcTrade(items);
}

void ProtocolGame::parsePlayerGoods(const InputMessagePtr& msg)
{
    std::vector<std::tuple<ItemPtr, uint16_t>> goods;

    uint64_t money = msg->get<uint64_t>();

    uint8_t size = msg->getByte();
    for(uint8_t i = 0; i < size; i++) {
        uint16_t itemId = msg->get<uint16_t>();
        uint16_t amount = msg->get<uint16_t>();

        goods.push_back(std::make_tuple(Item::create(itemId), amount));
    }

    g_game.processPlayerGoods(money, goods);
}

void ProtocolGame::parseCloseNpcTrade(const InputMessagePtr&)
{
    g_game.processCloseNpcTrade();
}

void ProtocolGame::parseOwnTrade(const InputMessagePtr& msg)
{
    std::string name = g_game.formatCreatureName(msg->getString());
    int count = msg->getByte();

    std::vector<ItemPtr> items(count);
    for(int i = 0; i < count; i++)
        items[i] = getItem(msg);

    g_game.processOwnTrade(name, items);
}

void ProtocolGame::parseCounterTrade(const InputMessagePtr& msg)
{
    std::string name = g_game.formatCreatureName(msg->getString());
    int count = msg->getByte();

    std::vector<ItemPtr> items(count);
    for(int i = 0; i < count; i++)
        items[i] = getItem(msg);

    g_game.processCounterTrade(name, items);
}

void ProtocolGame::parseCloseTrade(const InputMessagePtr&)
{
    g_game.processCloseTrade();
}

void ProtocolGame::parseWorldLight(const InputMessagePtr& msg)
{
    Light light;
    light.intensity = msg->getByte();
    light.color = msg->getByte();

    g_map.setLight(light);
}

void ProtocolGame::parseMagicEffect(const InputMessagePtr& msg)
{
    Position pos = getPosition(msg);
    int effectId;
    if(g_game.getFeature(Otc::GameMagicEffectU16))
        effectId = msg->get<uint16_t>();
    else
        effectId = msg->getByte();

    if(!g_things.isValidDatId(effectId, ThingCategoryEffect)) {
        g_logger.traceError(stdext::format("invalid effect id %d", effectId));
        return;
    }

    EffectPtr effect = EffectPtr(new Effect());
    effect->setId(effectId);
    g_map.addThing(effect, pos);
}

void ProtocolGame::parseAnimatedText(const InputMessagePtr& msg)
{
    Position position = getPosition(msg);
    int color = msg->getByte();
    std::string text = msg->getString();

    AnimatedTextPtr animatedText = AnimatedTextPtr(new AnimatedText);
    animatedText->setColor(color);
    animatedText->setText(text);
    g_map.addThing(animatedText, position);
}

void ProtocolGame::parseDistanceMissile(const InputMessagePtr& msg)
{
    Position fromPos = getPosition(msg);
    Position toPos = getPosition(msg);
    int shotId = msg->get<uint16_t>();

    if(!g_things.isValidDatId(shotId, ThingCategoryMissile)) {
        g_logger.traceError(stdext::format("invalid missile id %d", shotId));
        return;
    }

    MissilePtr missile = MissilePtr(new Missile());
    missile->setId(shotId);
    missile->setPath(fromPos, toPos);
    g_map.addThing(missile, fromPos);
}

void ProtocolGame::parseCreatureMark(const InputMessagePtr& msg)
{
    unsigned int id = msg->get<uint32_t>();
    int color = msg->getByte();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->addTimedSquare(color);
    else
        g_logger.traceError("could not get creature");
}

void ProtocolGame::parseTrappers(const InputMessagePtr& msg)
{
    int numTrappers = msg->getByte();

    if(numTrappers > 8)
        g_logger.traceError("too many trappers");

    for(int i=0;i<numTrappers;++i) {
        unsigned int id = msg->get<uint32_t>();
        CreaturePtr creature = g_map.getCreatureById(id);
        if(creature) {
            //TODO: set creature as trapper
        } else
            g_logger.traceError("could not get creature");
    }
}

void ProtocolGame::parseCreatureHealth(const InputMessagePtr& msg)
{
    unsigned int id = msg->get<uint32_t>();
    int healthPercent = msg->getByte();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setHealthPercent(healthPercent);

    // some servers has a bug in get spectators and sends unknown creatures updates
    // so this code is disabled
    /*
    else
        g_logger.traceError("could not get creature");
    */
}

void ProtocolGame::parseCreatureLight(const InputMessagePtr& msg)
{
    unsigned int id = msg->get<uint32_t>();

    Light light;
    light.intensity = msg->getByte();
    light.color = msg->getByte();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setLight(light);
    else
        g_logger.traceError("could not get creature");
}

void ProtocolGame::parseCreatureOutfit(const InputMessagePtr& msg)
{
    unsigned int id = msg->get<uint32_t>();
    Outfit outfit = getOutfit(msg);

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setOutfit(outfit);
    else
        g_logger.traceError("could not get creature");
}

void ProtocolGame::parseCreatureSpeed(const InputMessagePtr& msg)
{
    unsigned int id = msg->get<uint32_t>();

    int baseSpeed = msg->get<uint16_t>();

    int speed = msg->get<uint16_t>();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature) {
        creature->setSpeed(speed);
        if(baseSpeed != -1)
            creature->setBaseSpeed(baseSpeed);
    }

    // some servers has a bug in get spectators and sends unknown creatures updates
    // so this code is disabled
    /*
    else
        g_logger.traceError("could not get creature");
    */
}

void ProtocolGame::parseCreatureGenders(const InputMessagePtr& msg)
{
    unsigned int id = msg->get<uint32_t>();
    int gender = msg->getByte();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setGender(gender);
    else
        g_logger.traceError("could not get creature");
}

void ProtocolGame::parseCreatureShields(const InputMessagePtr& msg)
{
    unsigned int id = msg->get<uint32_t>();
    int shield = msg->getByte();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setShield(shield);
    else
        g_logger.traceError("could not get creature");
}

void ProtocolGame::parseCreatureUnpass(const InputMessagePtr& msg)
{
    unsigned int id = msg->get<uint32_t>();
    bool unpass = msg->getByte();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setPassable(!unpass);
    else
        g_logger.traceError("could not get creature");
}

void ProtocolGame::parseEditText(const InputMessagePtr& msg)
{
    unsigned int id = msg->get<uint32_t>();

    ItemPtr item = getItem(msg);
    int itemId = item->getId();

    int maxLength = msg->get<uint16_t>();
    std::string text = msg->getString();

    std::string writer = msg->getString();
    std::string date = "";
    if(g_game.getFeature(Otc::GameWritableDate))
        date = msg->getString();

    g_game.processEditText(id, itemId, maxLength, text, writer, date);
}

void ProtocolGame::parseEditList(const InputMessagePtr& msg)
{
    int doorId = msg->getByte();
    unsigned int id = msg->get<uint32_t>();
    const std::string& text = msg->getString();

    g_game.processEditList(id, doorId, text);
}

void ProtocolGame::parsePremiumTrigger(const InputMessagePtr& msg)
{
    int triggerCount = msg->getByte();
    std::vector<int> triggers;
    for(int i=0;i<triggerCount;++i) {
        triggers.push_back(msg->getByte());
    }
}

void ProtocolGame::parsePlayerInfo(const InputMessagePtr& msg)
{
    bool premium = msg->getByte(); // premium
    if(g_game.getFeature(Otc::GamePremiumExpiration))
        int premiumEx = msg->get<uint32_t>(); // premium expiration used for premium advertisement
    int vocation = msg->getByte(); // vocation

    int spellCount = msg->get<uint16_t>();
    std::vector<int> spells;
    for(int i=0;i<spellCount;++i)
        spells.push_back(msg->getByte()); // spell id

    m_localPlayer->setPremium(premium);
    m_localPlayer->setVocation(vocation);
    m_localPlayer->setSpells(spells);
}

void ProtocolGame::parsePlayerStats(const InputMessagePtr& msg)
{
    double health;
    double maxHealth;

    if(g_game.getFeature(Otc::GameDoubleHealth)) {
        health = msg->get<uint32_t>();
        maxHealth = msg->get<uint32_t>();
    } else {
        health = msg->get<uint16_t>();
        maxHealth = msg->get<uint16_t>();
    }

    double freeCapacity;
    if(g_game.getFeature(Otc::GameDoubleFreeCapacity))
        freeCapacity = msg->get<uint32_t>() / 100.0;
    else
        freeCapacity = msg->get<uint16_t>() / 100.0;

    double totalCapacity = 0;
    if(g_game.getFeature(Otc::GameTotalCapacity))
        totalCapacity = msg->get<uint32_t>() / 100.0;

    double experience;
    if(g_game.getFeature(Otc::GameDoubleExperience))
        experience = msg->get<uint64_t>();
    else
        experience = msg->get<uint32_t>();

    double level = msg->get<uint16_t>();
    double levelPercent = msg->getByte();

    if(g_game.getFeature(Otc::GameExperienceBonus)) {
        int baseXpGain = msg->get<uint16_t>();
        int voucherAddend = msg->get<uint16_t>();
        int grindingAddend = msg->get<uint16_t>();
        int storeBoostAddend = msg->get<uint16_t>();
        int huntingBoostFactor = msg->get<uint16_t>();
    }

    double mana;
    double maxMana;

    if(g_game.getFeature(Otc::GameDoubleHealth)) {
        mana = msg->get<uint32_t>();
        maxMana = msg->get<uint32_t>();
    } else {
        mana = msg->get<uint16_t>();
        maxMana = msg->get<uint16_t>();
    }

    double magicLevel = msg->getByte();

    double baseMagicLevel;
    if(g_game.getFeature(Otc::GameSkillsBase))
        baseMagicLevel = msg->getByte();
    else
        baseMagicLevel = magicLevel;

    double magicLevelPercent = msg->getByte();
    double soul = msg->getByte();
    double stamina = 0;
    if(g_game.getFeature(Otc::GamePlayerStamina))
        stamina = msg->get<uint16_t>();

    double baseSpeed = 0;
    if(g_game.getFeature(Otc::GameSkillsBase))
        baseSpeed = msg->get<uint16_t>();

    double regeneration = 0;
    if(g_game.getFeature(Otc::GamePlayerRegenerationTime))
        regeneration = msg->get<uint16_t>();

    int remainingStoreXpBoostSeconds = msg->get<uint16_t>();
    bool canBuyMoreStoreXpBoosts = msg->getByte();

    m_localPlayer->setHealth(health, maxHealth);
    m_localPlayer->setFreeCapacity(freeCapacity);
    m_localPlayer->setTotalCapacity(totalCapacity);
    m_localPlayer->setExperience(experience);
    m_localPlayer->setLevel(level, levelPercent);
    m_localPlayer->setMana(mana, maxMana);
    m_localPlayer->setMagicLevel(magicLevel, magicLevelPercent);
    m_localPlayer->setBaseMagicLevel(baseMagicLevel);
    m_localPlayer->setStamina(stamina);
    m_localPlayer->setSoul(soul);
    m_localPlayer->setBaseSpeed(baseSpeed);
    m_localPlayer->setRegenerationTime(regeneration);
}

void ProtocolGame::parsePlayerSkills(const InputMessagePtr& msg)
{
    for(int skill = 0; skill < Otc::LastSkill; skill++) {
        int level;

        if(g_game.getFeature(Otc::GameDoubleSkills))
            level = msg->get<uint16_t>();
        else
            level = msg->getByte();

        int baseLevel;
        if(g_game.getFeature(Otc::GameSkillsBase))
            if(g_game.getFeature(Otc::GameBaseSkillU16))
                baseLevel = msg->get<uint16_t>();
            else
                baseLevel = msg->getByte();
        else
            baseLevel = level;

        int levelPercent = 0;
        levelPercent = msg->getByte();

        m_localPlayer->setSkill((Otc::Skill)skill, level, levelPercent);
        m_localPlayer->setBaseSkill((Otc::Skill)skill, baseLevel);
    }
}

void ProtocolGame::parsePlayerState(const InputMessagePtr& msg)
{
    int states;
    if(g_game.getFeature(Otc::GamePlayerStateU16))
        states = msg->get<uint16_t>();
    else
        states = msg->getByte();

    m_localPlayer->setStates(states);
}

void ProtocolGame::parsePlayerCancelAttack(const InputMessagePtr& msg)
{
    unsigned int seq = 0;
    if(g_game.getFeature(Otc::GameAttackSeq))
        seq = msg->get<uint32_t>();

    g_game.processAttackCancel(seq);
}


void ProtocolGame::parsePlayerModes(const InputMessagePtr& msg)
{
    int fightMode = msg->getByte();
    int chaseMode = msg->getByte();
    bool safeMode = msg->getByte();

    int pvpMode = 0;
    if(g_game.getFeature(Otc::GamePVPMode))
        pvpMode = msg->getByte();

    g_game.processPlayerModes((Otc::FightModes)fightMode, (Otc::ChaseModes)chaseMode, safeMode, (Otc::PVPModes)pvpMode);
}

void ProtocolGame::parseSpellCooldown(const InputMessagePtr& msg)
{
    int spellId = msg->getByte();
    int delay = msg->get<uint32_t>();

    g_lua.callGlobalField("g_game", "onSpellCooldown", spellId, delay);
}

void ProtocolGame::parseSpellGroupCooldown(const InputMessagePtr& msg)
{
    int groupId = msg->getByte();
    int delay = msg->get<uint32_t>();

    g_lua.callGlobalField("g_game", "onSpellGroupCooldown", groupId, delay);
}

void ProtocolGame::parseMultiUseCooldown(const InputMessagePtr& msg)
{
    int delay = msg->get<uint32_t>();

    g_lua.callGlobalField("g_game", "onMultiUseCooldown", delay);
}

void ProtocolGame::parseTalk(const InputMessagePtr& msg)
{
    if(g_game.getFeature(Otc::GameMessageStatements))
        msg->get<uint32_t>(); // channel statement guid

    std::string name = g_game.formatCreatureName(msg->getString());

    int level = 0;
    if(g_game.getFeature(Otc::GameMessageLevel))
        level = msg->get<uint16_t>();

    Otc::MessageMode mode = Proto::translateMessageModeFromServer(msg->getByte());
    int channelId = 0;
    Position pos;

    switch(mode) {
        case Otc::MessageSay:
        case Otc::MessageWhisper:
        case Otc::MessageYell:
        case Otc::MessageMonsterSay:
        case Otc::MessageMonsterYell:
        case Otc::MessageNpcTo:
        case Otc::MessageBarkLow:
        case Otc::MessageBarkLoud:
        case Otc::MessageSpell:
        case Otc::MessageNpcFromStartBlock:
            pos = getPosition(msg);
            break;
        case Otc::MessageChannel:
        case Otc::MessageChannelManagement:
        case Otc::MessageChannelHighlight:
        case Otc::MessageGamemasterChannel:
            channelId = msg->get<uint16_t>();
            break;
        case Otc::MessageNpcFrom:
        case Otc::MessagePrivateFrom:
        case Otc::MessageGamemasterBroadcast:
        case Otc::MessageGamemasterPrivateFrom:
        case Otc::MessageRVRAnswer:
        case Otc::MessageRVRContinue:
            break;
        case Otc::MessageRVRChannel:
            msg->get<uint32_t>();
            break;
        default:
            stdext::throw_exception(stdext::format("unknown message mode %d", mode));
            break;
    }

    std::string text = msg->getString();

    g_game.processTalk(name, level, mode, text, channelId, pos);
}

void ProtocolGame::parseChannelList(const InputMessagePtr& msg)
{
    int count = msg->getByte();
    std::vector<std::tuple<int, std::string> > channelList;
    for(int i = 0; i < count; i++) {
        int id = msg->get<uint16_t>();
        std::string name = msg->getString();
        channelList.push_back(std::make_tuple(id, name));
    }

    g_game.processChannelList(channelList);
}

void ProtocolGame::parseOpenChannel(const InputMessagePtr& msg)
{
    int channelId = msg->get<uint16_t>();
    std::string name = msg->getString();

    if(g_game.getFeature(Otc::GameChannelPlayerList)) {
        int joinedPlayers = msg->get<uint16_t>();
        for(int i=0;i<joinedPlayers;++i)
            g_game.formatCreatureName(msg->getString()); // player name
        int invitedPlayers = msg->get<uint16_t>();
        for(int i=0;i<invitedPlayers;++i)
            g_game.formatCreatureName(msg->getString()); // player name
    }

    g_game.processOpenChannel(channelId, name);
}

void ProtocolGame::parseOpenPrivateChannel(const InputMessagePtr& msg)
{
    std::string name = g_game.formatCreatureName(msg->getString());

    g_game.processOpenPrivateChannel(name);
}

void ProtocolGame::parseOpenOwnPrivateChannel(const InputMessagePtr& msg)
{
    int channelId = msg->get<uint16_t>();
    std::string name = msg->getString();

    g_game.processOpenOwnPrivateChannel(channelId, name);
}

void ProtocolGame::parseCloseChannel(const InputMessagePtr& msg)
{
    int channelId = msg->get<uint16_t>();

    g_game.processCloseChannel(channelId);
}

void ProtocolGame::parseRuleViolationChannel(const InputMessagePtr& msg)
{
    int channelId = msg->get<uint16_t>();

    g_game.processRuleViolationChannel(channelId);
}

void ProtocolGame::parseRuleViolationRemove(const InputMessagePtr& msg)
{
    std::string name = msg->getString();

    g_game.processRuleViolationRemove(name);
}

void ProtocolGame::parseRuleViolationCancel(const InputMessagePtr& msg)
{
    std::string name = msg->getString();

    g_game.processRuleViolationCancel(name);
}

void ProtocolGame::parseRuleViolationLock(const InputMessagePtr& msg)
{
    g_game.processRuleViolationLock();
}

void ProtocolGame::parseTextMessage(const InputMessagePtr& msg)
{
    int code = msg->getByte();
    Otc::MessageMode mode = Proto::translateMessageModeFromServer(code);
    std::string text;

    switch(mode) {
        case Otc::MessageChannelManagement: {
            int channel = msg->get<uint16_t>();
            text = msg->getString();
            break;
        }
        case Otc::MessageGuild:
        case Otc::MessagePartyManagement:
        case Otc::MessageParty: {
            int channel = msg->get<uint16_t>();
            text = msg->getString();
            break;
        }
        case Otc::MessageDamageDealed:
        case Otc::MessageDamageReceived:
        case Otc::MessageDamageOthers: {
            Position pos = getPosition(msg);
            unsigned int value[2];
            int color[2];

            // physical damage
            value[0] = msg->get<uint32_t>();
            color[0] =  msg->getByte();

            // magic damage
            value[1] = msg->get<uint32_t>();
            color[1] = msg->getByte();
            text = msg->getString();

            for(int i=0;i<2;++i) {
                if(value[i] == 0)
                    continue;
                AnimatedTextPtr animatedText = AnimatedTextPtr(new AnimatedText);
                animatedText->setColor(color[i]);
                animatedText->setText(stdext::to_string(value[i]));
                g_map.addThing(animatedText, pos);
            }
            break;
        }
        case Otc::MessageHeal:
        case Otc::MessageMana:
        case Otc::MessageExp:
        case Otc::MessageHealOthers:
        case Otc::MessageExpOthers: {
            Position pos = getPosition(msg);
            unsigned int value = msg->get<uint32_t>();
            int color =  msg->getByte();
            text = msg->getString();

            AnimatedTextPtr animatedText = AnimatedTextPtr(new AnimatedText);
            animatedText->setColor(color);
            animatedText->setText(stdext::to_string(value));
            g_map.addThing(animatedText, pos);
            break;
        }
        case Otc::MessageInvalid:
            stdext::throw_exception(stdext::format("unknown message mode %d", mode));
            break;
        default:
            text = msg->getString();
            break;
    }

    g_game.processTextMessage(mode, text);
}

void ProtocolGame::parseCancelWalk(const InputMessagePtr& msg)
{
    Otc::Direction direction = (Otc::Direction)msg->getByte();

    g_game.processWalkCancel(direction);
}

void ProtocolGame::parseWalkWait(const InputMessagePtr& msg)
{
    int millis = msg->get<uint16_t>();
    m_localPlayer->lockWalk(millis);
}

void ProtocolGame::parseFloorChangeUp(const InputMessagePtr& msg)
{
    Position pos;
    if(g_game.getFeature(Otc::GameMapMovePosition))
        pos = getPosition(msg);
    else
        pos = g_map.getCentralPosition();
    AwareRange range = g_map.getAwareRange();
    pos.z--;

    int skip = 0;
    if(pos.z == Otc::SEA_FLOOR)
        for(int i = Otc::SEA_FLOOR - Otc::AWARE_UNDEGROUND_FLOOR_RANGE; i >= 0; i--)
            skip = setFloorDescription(msg, pos.x - range.left, pos.y - range.top, i, range.horizontal(), range.vertical(), 8 - i, skip);
    else if(pos.z > Otc::SEA_FLOOR)
        skip = setFloorDescription(msg, pos.x - range.left, pos.y - range.top, pos.z - Otc::AWARE_UNDEGROUND_FLOOR_RANGE, range.horizontal(), range.vertical(), 3, skip);

    pos.x++;
    pos.y++;
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseFloorChangeDown(const InputMessagePtr& msg)
{
    Position pos;
    if(g_game.getFeature(Otc::GameMapMovePosition))
        pos = getPosition(msg);
    else
        pos = g_map.getCentralPosition();
    AwareRange range = g_map.getAwareRange();
    pos.z++;

    int skip = 0;
    if(pos.z == Otc::UNDERGROUND_FLOOR) {
        int j, i;
        for(i = pos.z, j = -1; i <= pos.z + Otc::AWARE_UNDEGROUND_FLOOR_RANGE; ++i, --j)
            skip = setFloorDescription(msg, pos.x - range.left, pos.y - range.top, i, range.horizontal(), range.vertical(), j, skip);
    }
    else if(pos.z > Otc::UNDERGROUND_FLOOR && pos.z < Otc::MAX_Z-1)
        skip = setFloorDescription(msg, pos.x - range.left, pos.y - range.top, pos.z + Otc::AWARE_UNDEGROUND_FLOOR_RANGE, range.horizontal(), range.vertical(), -3, skip);

    pos.x--;
    pos.y--;
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseOpenOutfitWindow(const InputMessagePtr& msg)
{
    Outfit currentOutfit = getOutfit(msg);
    std::vector<std::tuple<int, std::string, int> > outfitList;

    if(g_game.getFeature(Otc::GameNewOutfitProtocol)) {
        int outfitCount = msg->getByte();
        for(int i = 0; i < outfitCount; i++) {
            int outfitId = msg->get<uint16_t>();
            std::string outfitName = msg->getString();
            int outfitAddons = msg->getByte();

            outfitList.push_back(std::make_tuple(outfitId, outfitName, outfitAddons));
        }
    } else {
        int outfitStart, outfitEnd;
        if(g_game.getFeature(Otc::GameLooktypeU16)) {
            outfitStart = msg->get<uint16_t>();
            outfitEnd = msg->get<uint16_t>();
        } else {
            outfitStart = msg->getByte();
            outfitEnd = msg->getByte();
        }

        for(int i = outfitStart; i <= outfitEnd; i++)
            outfitList.push_back(std::make_tuple(i, "", 0));
    }

    std::vector<std::tuple<int, std::string> > mountList;
    if(g_game.getFeature(Otc::GamePlayerMounts)) {
        int mountCount = msg->getByte();
        for(int i = 0; i < mountCount; ++i) {
            int mountId = msg->get<uint16_t>(); // mount type
            std::string mountName = msg->getString(); // mount name

            mountList.push_back(std::make_tuple(mountId, mountName));
        }
    }

    g_game.processOpenOutfitWindow(currentOutfit, outfitList, mountList);
}

void ProtocolGame::parseVipAdd(const InputMessagePtr& msg)
{
    unsigned int id, iconId = 0, status;
    std::string name, desc = "";
    bool notifyLogin = false;

    id = msg->get<uint32_t>();
    name = g_game.formatCreatureName(msg->getString());
    if(g_game.getFeature(Otc::GameAdditionalVipInfo)) {
        desc = msg->getString();
        iconId = msg->get<uint32_t>();
        notifyLogin = msg->getByte();
    }
    status = msg->getByte();

    g_game.processVipAdd(id, name, status, desc, iconId, notifyLogin);
}

void ProtocolGame::parseVipState(const InputMessagePtr& msg)
{
    unsigned int id = msg->get<uint32_t>();
    if(g_game.getFeature(Otc::GameLoginPending)) {
        unsigned int status = msg->getByte();
        g_game.processVipStateChange(id, status);
    }
    else {
        g_game.processVipStateChange(id, 1);
    }
}

void ProtocolGame::parseVipLogout(const InputMessagePtr& msg)
{
    unsigned int id = msg->get<uint32_t>();
    g_game.processVipStateChange(id, 0);
}

void ProtocolGame::parseTutorialHint(const InputMessagePtr& msg)
{
    int id = msg->getByte();
    g_game.processTutorialHint(id);
}

void ProtocolGame::parseAutomapFlag(const InputMessagePtr& msg)
{
    Position pos = getPosition(msg);
    int icon = msg->getByte();
    std::string description = msg->getString();

    bool remove = false;
    if(g_game.getFeature(Otc::GameMinimapRemove))
        remove = msg->getByte() != 0;

    if(!remove)
        g_game.processAddAutomapFlag(pos, icon, description);
    else
        g_game.processRemoveAutomapFlag(pos, icon, description);
}

void ProtocolGame::parseQuestLog(const InputMessagePtr& msg)
{
    std::vector<std::tuple<int, std::string, bool> > questList;
    int questsCount = msg->get<uint16_t>();
    for(int i = 0; i < questsCount; i++) {
        int id = msg->get<uint16_t>();
        std::string name = msg->getString();
        bool completed = msg->getByte();
        questList.push_back(std::make_tuple(id, name, completed));
    }

    g_game.processQuestLog(questList);
}

void ProtocolGame::parseQuestLine(const InputMessagePtr& msg)
{
    std::vector<std::tuple<std::string, std::string>> questMissions;
    int questId = msg->get<uint16_t>();
    int missionCount = msg->getByte();
    for(int i = 0; i < missionCount; i++) {
        std::string missionName = msg->getString();
        std::string missionDescrition = msg->getString();
        questMissions.push_back(std::make_tuple(missionName, missionDescrition));
    }

    g_game.processQuestLine(questId, questMissions);
}

void ProtocolGame::parseChannelEvent(const InputMessagePtr& msg)
{
    msg->get<uint16_t>(); // channel id
    g_game.formatCreatureName(msg->getString()); // player name
    msg->getByte(); // event type
}

void ProtocolGame::parseItemInfo(const InputMessagePtr& msg)
{
    std::vector<std::tuple<ItemPtr, std::string>> list;
    int size = msg->getByte();
    for(int i=0;i<size;++i) {
        ItemPtr item(new Item);
        item->setId(msg->get<uint16_t>());
        item->setCountOrSubType(msg->get<uint16_t>());

        std::string desc = msg->getString();
        list.push_back(std::make_tuple(item, desc));
    }

    g_lua.callGlobalField("g_game", "onItemInfo", list);
}

void ProtocolGame::parsePlayerInventory(const InputMessagePtr& msg)
{
    int size = msg->get<uint16_t>();
    for(int i=0;i<size;++i) {
        msg->get<uint16_t>(); // id
        msg->getByte(); // subtype
        msg->get<uint16_t>(); // count
    }
}

void ProtocolGame::parseModalDialog(const InputMessagePtr& msg)
{
    uint32_t id = msg->get<uint32_t>();
    std::string title = msg->getString();
    std::string message = msg->getString();

    int sizeButtons = msg->getByte();
    std::vector<std::tuple<int, std::string> > buttonList;
    for(int i = 0; i < sizeButtons; ++i) {
        std::string value = msg->getString();
        int id = msg->getByte();
        buttonList.push_back(std::make_tuple(id, value));
    }

    int sizeChoices = msg->getByte();
    std::vector<std::tuple<int, std::string> > choiceList;
    for(int i = 0; i < sizeChoices; ++i) {
        std::string value = msg->getString();
        int id = msg->getByte();
        choiceList.push_back(std::make_tuple(id, value));
    }

    int escapeButton = msg->getByte();
    int enterButton = msg->getByte();

    bool priority = msg->getByte() == 0x01;

    g_game.processModalDialog(id, title, message, buttonList, enterButton, escapeButton, choiceList, priority);
}

void ProtocolGame::parseExtendedOpcode(const InputMessagePtr& msg)
{
    int opcode = msg->getByte();
    std::string buffer = msg->getString();

    if(opcode == 0)
        m_enableSendExtendedOpcode = true;
    else if(opcode == 2)
        parsePingBack(msg);
    else
        callLuaField("onExtendedOpcode", opcode, buffer);
}

void ProtocolGame::parseChangeMapAwareRange(const InputMessagePtr& msg)
{
    int xrange = msg->getByte();
    int yrange = msg->getByte();

    AwareRange range;
    range.left = xrange/2 - ((xrange+1) % 2);
    range.right = xrange/2;
    range.top = yrange/2 - ((yrange+1) % 2);
    range.bottom = yrange/2;

    g_map.setAwareRange(range);
    g_lua.callGlobalField("g_game", "onMapChangeAwareRange", xrange, yrange);
}

void ProtocolGame::parseCreaturesMark(const InputMessagePtr& msg)
{
    int len = 1;

    for(int i=0; i<len; ++i) {
        uint32_t id = msg->get<uint32_t>();
        bool isPermanent = msg->getByte() != 1;
        uint8_t markType = msg->getByte();

        CreaturePtr creature = g_map.getCreatureById(id);
        if(creature) {
            if(isPermanent) {
                if(markType == 0xff)
                    creature->hideStaticSquare();
                else
                    creature->showStaticSquare(Color::from8bit(markType));
            } else
                creature->addTimedSquare(markType);
        } else
            g_logger.traceError("could not get creature");
    }
}

void ProtocolGame::parseCreatureType(const InputMessagePtr& msg)
{
    uint32_t id = msg->get<uint32_t>();
    uint8_t type = msg->getByte();

    CreaturePtr creature = g_map.getCreatureById(id);
    if(creature)
        creature->setType(type);
    else
        g_logger.traceError("could not get creature");
}

void ProtocolGame::setMapDescription(const InputMessagePtr& msg, int x, int y, int z, int width, int height)
{
    int startz, endz, zstep;

    if(z > Otc::SEA_FLOOR) {
        startz = z - Otc::AWARE_UNDEGROUND_FLOOR_RANGE;
        endz = std::min<int>(z + Otc::AWARE_UNDEGROUND_FLOOR_RANGE, (int)Otc::MAX_Z);
        zstep = 1;
    }
    else {
        startz = Otc::SEA_FLOOR;
        endz = 0;
        zstep = -1;
    }

    int skip = 0;
    for(int nz = startz; nz != endz + zstep; nz += zstep)
        skip = setFloorDescription(msg, x, y, nz, width, height, z - nz, skip);
}

int ProtocolGame::setFloorDescription(const InputMessagePtr& msg, int x, int y, int z, int width, int height, int offset, int skip)
{
    for(int nx = 0; nx < width; nx++) {
        for(int ny = 0; ny < height; ny++) {
            Position tilePos(x + nx + offset, y + ny + offset, z);
            if(skip == 0)
                skip = setTileDescription(msg, tilePos);
            else {
                g_map.cleanTile(tilePos);
                skip--;
            }
        }
    }
    return skip;
}

int ProtocolGame::setTileDescription(const InputMessagePtr& msg, Position position)
{
    g_map.cleanTile(position);

    bool gotEffect = false;
    for(int stackPos=0;stackPos<256;stackPos++) {
        if(msg->peekU16()  >= 0xff00)
            return msg->get<uint16_t>() & 0xff;

        if(g_game.getFeature(Otc::GameEnvironmentEffect) && !gotEffect) {
            msg->get<uint16_t>(); // environment effect
            gotEffect = true;
            continue;
        }

        if(stackPos > 10)
            g_logger.traceError(stdext::format("too many things, pos=%s, stackpos=%d", stdext::to_string(position), stackPos));

        ThingPtr thing = getThing(msg);
        g_map.addThing(thing, position, stackPos);
    }

    return 0;
}
Outfit ProtocolGame::getOutfit(const InputMessagePtr& msg)
{
    Outfit outfit;

    int lookType;
    if(g_game.getFeature(Otc::GameLooktypeU16))
        lookType = msg->get<uint16_t>();
    else
        lookType = msg->getByte();

    if(lookType != 0) {
        outfit.setCategory(ThingCategoryCreature);
        int head = msg->getByte();
        int body = msg->getByte();
        int legs = msg->getByte();
        int feet = msg->getByte();
        int addons = 0;
        if(g_game.getFeature(Otc::GamePlayerAddons))
            addons = msg->getByte();

        if(!g_things.isValidDatId(lookType, ThingCategoryCreature)) {
            g_logger.traceError(stdext::format("invalid outfit looktype %d", lookType));
            lookType = 0;
        }

        outfit.setId(lookType);
        outfit.setHead(head);
        outfit.setBody(body);
        outfit.setLegs(legs);
        outfit.setFeet(feet);
        outfit.setAddons(addons);
    }
    else {
        int lookTypeEx = msg->get<uint16_t>();
        if(lookTypeEx == 0) {
            outfit.setCategory(ThingCategoryEffect);
            outfit.setAuxId(13); // invisible effect id
        }
        else {
            if(!g_things.isValidDatId(lookTypeEx, ThingCategoryItem)) {
                g_logger.traceError(stdext::format("invalid outfit looktypeex %d", lookTypeEx));
                lookTypeEx = 0;
            }
            outfit.setCategory(ThingCategoryItem);
            outfit.setAuxId(lookTypeEx);
        }
    }

    if(g_game.getFeature(Otc::GamePlayerMounts)) {
        int mount = msg->get<uint16_t>();
        outfit.setMount(mount);
    }

    return outfit;
}

ThingPtr ProtocolGame::getThing(const InputMessagePtr& msg)
{
    ThingPtr thing;

    int id = msg->get<uint16_t>();

    if(id == 0)
        stdext::throw_exception("invalid thing id");
    else if(id == Proto::UnknownCreature || id == Proto::OutdatedCreature || id == Proto::Creature)
        thing = getCreature(msg, id);
    else if(id == Proto::StaticText) // otclient only
        thing = getStaticText(msg, id);
    else // item
        thing = getItem(msg, id);

    return thing;
}

ThingPtr ProtocolGame::getMappedThing(const InputMessagePtr& msg)
{
    ThingPtr thing;
    uint16_t x = msg->get<uint16_t>();

    if(x != 0xffff) {
        Position pos;
        pos.x = x;
        pos.y = msg->get<uint16_t>();
        pos.z = msg->getByte();
        uint8_t stackpos = msg->getByte();
        assert(stackpos != 255);
        thing = g_map.getThing(pos, stackpos);
        if(!thing)
            g_logger.traceError(stdext::format("no thing at pos:%s, stackpos:%d", stdext::to_string(pos), stackpos));
    } else {
        uint32_t id = msg->get<uint32_t>();
        thing = g_map.getCreatureById(id);
        if(!thing)
            g_logger.traceError(stdext::format("no creature with id %u", id));
    }

    return thing;
}

CreaturePtr ProtocolGame::getCreature(const InputMessagePtr& msg, int type)
{
    if(type == 0)
        type = msg->get<uint16_t>();

    CreaturePtr creature;
    bool known = (type != Proto::UnknownCreature);
    if(type == Proto::OutdatedCreature || type == Proto::UnknownCreature) {
        if(known) {
            unsigned int id = msg->get<uint32_t>();
            creature = g_map.getCreatureById(id);
            if(!creature)
                g_logger.traceError("server said that a creature is known, but it's not");
        } else {
            unsigned int removeId = msg->get<uint32_t>();
            g_map.removeCreatureById(removeId);

            unsigned int id =msg->get<uint32_t>();

            int creatureType = msg->getByte();

            std::string name = g_game.formatCreatureName(msg->getString());

            if(id == m_localPlayer->getId())
                creature = m_localPlayer;
            else if(creatureType == Proto::CreatureTypePlayer) {
                // fixes a bug server side bug where GameInit is not sent and local player id is unknown
                if(m_localPlayer->getId() == 0 && name == m_localPlayer->getName())
                    creature = m_localPlayer;
                else
                    creature = PlayerPtr(new Player);
            }
            else if(creatureType == Proto::CreatureTypeMonster)
                creature = MonsterPtr(new Monster);
            else if(creatureType == Proto::CreatureTypeNpc)
                creature = NpcPtr(new Npc);
            else
                g_logger.traceError("creature type is invalid");

            if(creature) {
                creature->setId(id);
                creature->setName(name);

                g_map.addCreature(creature);
            }
        }

        int nameColor = msg->getByte();
        int healthPercent = msg->getByte();
        Otc::Direction direction = (Otc::Direction)msg->getByte();
        Outfit outfit = getOutfit(msg);

        Light light;
        light.intensity = msg->getByte();
        light.color = msg->getByte();

        int speed = msg->get<uint16_t>();
        int gender = msg->getByte();
        int shield = msg->getByte();

        // emblem is sent only when the creature is not known
        int8_t emblem = -1;
        int8_t creatureType = -1;
        int8_t icon = -1;
        bool unpass = true;
        uint8_t mark;

        if(g_game.getFeature(Otc::GameCreatureEmblems) && !known)
            emblem = msg->getByte();

        if(g_game.getFeature(Otc::GameThingMarks)) {
            creatureType = msg->getByte();
        }

        if(g_game.getFeature(Otc::GameCreatureIcons)) {
            icon = msg->getByte();
        }

        if(g_game.getFeature(Otc::GameThingMarks)) {
            mark = msg->getByte(); // mark
            msg->get<uint16_t>(); // helpers

            if(creature) {
                if(mark == 0xff)
                    creature->hideStaticSquare();
                else
                    creature->showStaticSquare(Color::from8bit(mark));
            }
        }

        unpass = msg->getByte();

        if(creature) {
            if(creatureType != -1)
                creature->setType(creatureType);

            creature->setNameColor(nameColor);
            creature->setHealthPercent(healthPercent);
            creature->setDirection(direction);
            creature->setOutfit(outfit);
            creature->setSpeed(speed);
            creature->setGender(gender);
            creature->setShield(shield);
            creature->setPassable(!unpass);
            creature->setLight(light);

            if(emblem != -1)
                creature->setEmblem(emblem);

            if(icon != -1)
                creature->setIcon(icon);

            if(creature == m_localPlayer && !m_localPlayer->isKnown())
                m_localPlayer->setKnown(true);
        }
    } else if(type == Proto::Creature) {
        unsigned int id = msg->get<uint32_t>();
        creature = g_map.getCreatureById(id);

        if(!creature)
            g_logger.traceError("invalid creature");

        Otc::Direction direction = (Otc::Direction)msg->getByte();
        if(creature)
            creature->turn(direction);

        bool unpass = msg->getByte();

        if(creature)
            creature->setPassable(!unpass);

    } else {
        stdext::throw_exception("invalid creature opcode");
    }

    return creature;
}

ItemPtr ProtocolGame::getItem(const InputMessagePtr& msg, int id)
{
    if(id == 0)
        id = msg->get<uint16_t>();

    ItemPtr item = Item::create(id);
    if(item->getId() == 0)
        stdext::throw_exception(stdext::format("unable to create item with invalid id %d", id));

    if(g_game.getFeature(Otc::GameThingMarks)) {
        msg->getByte(); // mark
    }

    if(item->isStackable() || item->isFluidContainer() || item->isSplash() || item->isChargeable())
        item->setCountOrSubType(msg->get<uint16_t>());

    if(g_game.getFeature(Otc::GameItemAnimationPhase)) {
        if(item->getAnimationPhases() > 1) {
            // 0x00 => automatic phase
            // 0xFE => random phase
            // 0xFF => async phase
            msg->getByte();
            //item->setPhase(msg->getByte());
        }
    }

    return item;
}

StaticTextPtr ProtocolGame::getStaticText(const InputMessagePtr& msg, int id)
{
    int colorByte = msg->getByte();
    Color color = Color::from8bit(colorByte);
    std::string fontName = msg->getString();
    std::string text = msg->getString();
    StaticTextPtr staticText = StaticTextPtr(new StaticText);
    staticText->setText(text);
    staticText->setFont(fontName);
    staticText->setColor(color);
    return staticText;
}

Position ProtocolGame::getPosition(const InputMessagePtr& msg)
{
    uint16_t x = msg->get<uint16_t>();
    uint16_t y = msg->get<uint16_t>();
    uint8_t z = msg->getByte();

    return Position(x, y, z);
}
