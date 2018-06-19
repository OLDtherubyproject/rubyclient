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

#include "protocolcodes.h"

namespace Proto {

std::map<uint8_t, uint8_t> messageModesMap;

void buildMessageModesMap(int version) {
    messageModesMap.clear();

    messageModesMap[Otc::MessageMana] = 43;
    messageModesMap[Otc::MessageNone] = 0;
    messageModesMap[Otc::MessageSay] = 1;
    messageModesMap[Otc::MessageWhisper] = 2;
    messageModesMap[Otc::MessageYell] = 3;
    messageModesMap[Otc::MessagePrivateFrom] = 4;
    messageModesMap[Otc::MessagePrivateTo] = 5;
    messageModesMap[Otc::MessageChannelManagement] = 6;
    messageModesMap[Otc::MessageChannel] = 7;
    messageModesMap[Otc::MessageChannelHighlight] = 8;
    messageModesMap[Otc::MessageSpell] = 9;
    messageModesMap[Otc::MessageNpcFromStartBlock] = 10;
    messageModesMap[Otc::MessageNpcFrom] = 11;
    messageModesMap[Otc::MessageNpcTo] = 12;
    messageModesMap[Otc::MessageGamemasterBroadcast] = 13;
    messageModesMap[Otc::MessageGamemasterChannel] = 14;
    messageModesMap[Otc::MessageGamemasterPrivateFrom] = 15;
    messageModesMap[Otc::MessageGamemasterPrivateTo] = 16;
    messageModesMap[Otc::MessageLogin] = 17;
    messageModesMap[Otc::MessageWarning] = 18; // Admin
    messageModesMap[Otc::MessageGame] = 19;
    messageModesMap[Otc::MessageGameHighlight] = 20;
    messageModesMap[Otc::MessageFailure] = 21;
    messageModesMap[Otc::MessageLook] = 22;
    messageModesMap[Otc::MessageDamageDealed] = 23;
    messageModesMap[Otc::MessageDamageReceived] = 24;
    messageModesMap[Otc::MessageHeal] = 25;
    messageModesMap[Otc::MessageExp] = 26;
    messageModesMap[Otc::MessageDamageOthers] = 27;
    messageModesMap[Otc::MessageHealOthers] = 28;
    messageModesMap[Otc::MessageExpOthers] = 29;
    messageModesMap[Otc::MessageStatus] = 30;
    messageModesMap[Otc::MessageLoot] = 31;
    messageModesMap[Otc::MessageTradeNpc] = 32;
    messageModesMap[Otc::MessageGuild] = 33;
    messageModesMap[Otc::MessagePartyManagement] = 34;
    messageModesMap[Otc::MessageParty] = 35;
    messageModesMap[Otc::MessageBarkLow] = 36;
    messageModesMap[Otc::MessageBarkLoud] = 37;
    messageModesMap[Otc::MessageReport] = 38;
    messageModesMap[Otc::MessageHotkeyUse] = 39;
    messageModesMap[Otc::MessageTutorialHint] = 40;
    messageModesMap[Otc::MessageThankyou] = 41;
    messageModesMap[Otc::MessageMarket] = 42;
}

Otc::MessageMode translateMessageModeFromServer(uint8_t mode)
{
    auto it = std::find_if(messageModesMap.begin(), messageModesMap.end(), [=] (const std::pair<uint8_t, uint8_t>& p) { return p.second == mode; });
    if(it != messageModesMap.end())
        return (Otc::MessageMode)it->first;
    return Otc::MessageInvalid;
}

uint8_t translateMessageModeToServer(Otc::MessageMode mode)
{
    if(mode < 0 || mode >= Otc::LastMessage)
        return Otc::MessageInvalid;
    auto it = messageModesMap.find(mode);
    if(it != messageModesMap.end())
        return it->second;
    return Otc::MessageInvalid;
}

}
