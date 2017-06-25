/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_ENTITIES_LASER_REPAIR_H
#define GAME_SERVER_ENTITIES_LASER_REPAIR_H

#include <game/server/entity.h>

class CLaserRepair : public CEntity
{
public:
	CLaserRepair(CGameWorld *pGameWorld, vec2 Pos, vec2 Direction, float StartEnergy, int Owner);

	virtual void Reset();
	virtual void Tick();
	virtual void Snap(int SnappingClient);

protected:
	bool HitCharacter(vec2 From, vec2 To);
	bool HitTurret(vec2 From, vec2 To);

private:
	vec2 m_From;
	vec2 m_Dir;
	vec2 m_Pos;
	float m_Energy;
	int m_Owner;
	float m_RepairTimer;
};

#endif
