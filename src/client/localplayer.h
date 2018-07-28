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

#ifndef LOCALPLAYER_H
#define LOCALPLAYER_H

#include "player.h"
#include <framework/sound/soundmanager.h>

// @bindclass
class LocalPlayer : public Player
{
    enum {
        PREWALK_TIMEOUT = 1000
    };

public:
    LocalPlayer();

    void unlockWalk() { m_walkLockExpiration = 0; }
    void lockWalk(int millis = 250);
    void stopAutoWalk();
    bool autoWalk(const Position& destination);
    bool canWalk(Otc::Direction direction);

    void setStates(int states);
    void setSkill(Otc::Skill skill, int level, int levelPercent);
    void setBaseSkill(Otc::Skill skill, int baseLevel);
    void setHealth(double health, double maxHealth);
    void setFreeCapacity(double freeCapacity);
    void setTotalCapacity(double totalCapacity);
    void setExperience(double experience);
    void setLevel(double level, double levelPercent);
    void setPokemonHealth(double getPokemonHealth, double maxPokemonHealth);
    void setPokemonCount(double pokemonCount);
    void setStamina(double stamina);
    void setKnown(bool known) { m_known = known; }
    void setPendingGame(bool pending) { m_pending = pending; }
    void setInventoryItem(Otc::InventorySlot inventory, const ItemPtr& item);
    void setProfession(int profession);
    void setClan(int clan);
    void setPremium(bool premium);
    void setListeningSound(int sound);
    void setRegenerationTime(double regenerationTime);
    void setBlessings(int blessings);

    int getStates() { return m_states; }
    int getSkillLevel(Otc::Skill skill) { return m_skillsLevel[skill]; }
    int getSkillBaseLevel(Otc::Skill skill) { return m_skillsBaseLevel[skill]; }
    int getSkillLevelPercent(Otc::Skill skill) { return m_skillsLevelPercent[skill]; }
    int getProfession() { return m_profession; }
    int getClan() { return m_clan; }
    double getHealth() { return m_health; }
    double getMaxHealth() { return m_maxHealth; }
    double getFreeCapacity() { return m_freeCapacity; }
    double getTotalCapacity() { return m_totalCapacity; }
    double getExperience() { return m_experience; }
    double getLevel() { return m_level; }
    double getLevelPercent() { return m_levelPercent; }
    double getPokemonHealth() { return m_pokemonHealth; }
    double getPokemonHealthMax() { return m_maxPokemonHealth; }
    double getPokemonCount() { return m_pokemonCount; }
    double getStamina() { return m_stamina; }
    double getRegenerationTime() { return m_regenerationTime; }
    ItemPtr getInventoryItem(Otc::InventorySlot inventory) { return m_inventoryItems[inventory]; }
    int getBlessings() { return m_blessings; }
    int getSound() { return m_sound; }
    int getListeningSound() { return m_sound; }

    bool hasSight(const Position& pos);
    bool isKnown() { return m_known; }
    bool isPreWalking() { return m_preWalking; }
    bool isAutoWalking() { return m_autoWalkDestination.isValid(); }
    bool isServerWalking() { return m_serverWalking; }
    bool isPremium() { return m_premium; }
    bool isPendingGame() { return m_pending; }

    LocalPlayerPtr asLocalPlayer() { return static_self_cast<LocalPlayer>(); }
    bool isLocalPlayer() { return true; }

    virtual void onAppear();
    virtual void onPositionChange(const Position& newPos, const Position& oldPos);

protected:
    void walk(const Position& oldPos, const Position& newPos);
    void preWalk(Otc::Direction direction);
    void cancelWalk(Otc::Direction direction = Otc::InvalidDirection);
    void stopWalk();

    friend class Game;

protected:
    void updateWalkOffset(int totalPixelsWalked);
    void updateWalk();
    void terminateWalk();

private:
    // walk related
    Position m_lastPrewalkDestination;
    Position m_autoWalkDestination;
    Position m_lastAutoWalkPosition;
    ScheduledEventPtr m_serverWalkEndEvent;
    ScheduledEventPtr m_autoWalkContinueEvent;
    ticks_t m_walkLockExpiration;
    stdext::boolean<false> m_preWalking;
    stdext::boolean<true> m_lastPrewalkDone;
    stdext::boolean<false> m_secondPreWalk;
    stdext::boolean<false> m_serverWalking;
    stdext::boolean<false> m_knownCompletePath;

    stdext::boolean<false> m_premium;
    stdext::boolean<false> m_known;
    stdext::boolean<false> m_pending;

    ItemPtr m_inventoryItems[Otc::LastInventorySlot];
    Timer m_idleTimer;

    std::array<int, Otc::LastSkill> m_skillsLevel;
    std::array<int, Otc::LastSkill> m_skillsBaseLevel;
    std::array<int, Otc::LastSkill> m_skillsLevelPercent;

    int m_states;
    int m_profession;
    int m_clan;
    int m_blessings;
    int m_sound;

    double m_health;
    double m_maxHealth;
    double m_freeCapacity;
    double m_totalCapacity;
    double m_experience;
    double m_level;
    double m_levelPercent;
    double m_pokemonHealth;
    double m_maxPokemonHealth;
    double m_pokemonCount;
    double m_stamina;
    double m_regenerationTime;
};

#endif
