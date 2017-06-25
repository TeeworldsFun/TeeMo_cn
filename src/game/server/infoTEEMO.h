/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_ENTITIES_CHARACTER_TEEMO_H
#define GAME_SERVER_ENTITIES_CHARACTER_TEEMO_H

#include <game/server/entity.h>
#include <game/generated/server_data.h>
#include <game/generated/protocol.h>

#include <game/gamecore.h>

enum {
    ROLE_ENGINEER=0,
    ROLE_SPY,
    ROLE_MEDIC,
    ROLE_GENERAL,
    ROLE_SOLDIER,
    MAX_ROLES,

    BONUS_FLY=2<<0,
    BONUS_GOD=2<<1,
    BONUS_HOOKER=2<<2,
    BONUS_INVISIBLE=2<<3
};

class CInfoTEEMO
{
public:
	CInfoTEEMO()
	{
        Reset();
	}

    int GetRole() const { return m_Role; }
    void SetRole(int role) { m_Role = role; }

    int GetRealTeam() const { return m_RealTeam; }
    void SetRealTeam(int team) { m_RealTeam = team; }

    bool HaveBonus(unsigned char bonus) { return m_Bonus&bonus; }
    void AddBonus(unsigned char bonus) {  m_Bonus |= bonus; }
    void DelBonus(unsigned char bonus) { m_Bonus &= ~bonus; }
    void ClearBonus() { m_Bonus = 0; }

    void Reset() {
        m_Role = ROLE_SOLDIER;
        m_RealTeam = TEAM_BLUE;
        m_Bonus = 0;
        m_Kills = 0;
        m_Deaths = 0;
    }

    unsigned int m_Kills;
    unsigned int m_Deaths;
    float m_FlyBonusTimer;
    float m_GodBonusTimer;
    float m_HookerBonusTimer;
    float m_InvisibleBonusTimer;

private:
    int m_Role;
    int m_RealTeam;
    unsigned char m_Bonus;
};

#endif

