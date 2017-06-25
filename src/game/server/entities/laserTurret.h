/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_ENTITIES_LASER_TURRET_H
#define GAME_SERVER_ENTITIES_LASER_TURRET_H

#include <game/server/entity.h>

class CLaserTurret : public CEntity
{
public:
	CLaserTurret(CGameWorld *pGameWorld, vec2 Pos, int Owner);

    float m_BuildProcess;

	virtual void Reset();
	virtual void Tick();
	virtual void Snap(int SnappingClient);

	int GetHealth() const { return m_Health; }
	void SetHealth(int health);
	int GetOwner() const { return m_Owner; }
	void TakeDamage(int damage, int pID);

	CPickup *m_pIndicatorPickup;

protected:
	bool HitCharacter();

private:
	int m_Owner;
	float m_ShootTick;
	int m_Health;
	float m_DamageTakenTick;
	int m_DamageTaken;
};

#endif
