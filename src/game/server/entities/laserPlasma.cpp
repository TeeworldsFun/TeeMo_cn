/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "laserPlasma.h"
#include "laserTurret.h"

CLaserPlasma::CLaserPlasma(CGameWorld *pGameWorld, int Type, int Owner, vec2 Pos, vec2 Dir, int Span,
		int Damage, bool Explosive, float Force, int SoundImpact, int Weapon)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_PROJECTILE)
{
	m_Type = Type;
	m_Pos = Pos;
	m_Direction = Dir;
	m_LifeSpan = Span;
	m_Owner = Owner;
	m_Force = Force;
	m_Damage = Damage;
	m_SoundImpact = SoundImpact;
	m_Weapon = Weapon;
	m_StartTick = Server()->Tick();
	m_Explosive = Explosive;
	m_Length = 0;

	GameWorld()->InsertEntity(this);
}

void CLaserPlasma::Reset()
{
	GameServer()->m_World.DestroyEntity(this);
}

vec2 CLaserPlasma::GetPos(float Time)
{
	float Speed = GameServer()->Tuning()->m_PlasmaRifleSpeed;

	return CalcPos(m_Pos, m_Direction, 0.0f, Speed, Time);
}


void CLaserPlasma::Tick()
{
	float Pt = (Server()->Tick()-m_StartTick-1)/(float)Server()->TickSpeed();
	float Ct = (Server()->Tick()-m_StartTick)/(float)Server()->TickSpeed();
	vec2 PrevPos = GetPos(Pt);
	vec2 CurPos = GetPos(Ct);
	vec2 FinPos;
	int Collide = GameServer()->Collision()->IntersectLine(PrevPos, CurPos, &CurPos, &FinPos);
	CCharacter *OwnerChar = GameServer()->GetPlayerChar(m_Owner);
	CCharacter *TargetChr = GameServer()->m_World.IntersectCharacter(PrevPos, CurPos, 6.0f, CurPos, OwnerChar);
	m_Pos = PrevPos;

	m_LifeSpan--;
	m_Length+=3;

	if (!Collide)
    {
        CEntity *apEnts[1];
        int Num = GameServer()->m_World.FindEntities(CurPos, 22.0f, (CEntity**)apEnts, 1, CGameWorld::ENTTYPE_LASER);
        if (Num > 0)
        {
            CLaserTurret *pLaserTurret = dynamic_cast<CLaserTurret*>(apEnts[0]);
            if (pLaserTurret)
            {
                pLaserTurret->TakeDamage(m_Damage, m_Owner);
                if (pLaserTurret->GetHealth() > 0.0f)
                {
                    char buff[255]={0};
                    str_format(buff, sizeof(buff), "TURRET HEALTH: %d", pLaserTurret->GetHealth());
                    GameServer()->SendBroadcast(buff, m_Owner);
                }
                else
                    GameServer()->SendBroadcast("TURRET DESTROYED!", m_Owner);

                Collide = true;
            }
        }
    }

	if(TargetChr || Collide || m_LifeSpan < 0 || GameLayerClipped(CurPos))
	{
		if(m_LifeSpan >= 0)
			GameServer()->CreateSound(CurPos, m_SoundImpact);

		if(m_Explosive)
		{
            GameServer()->CreateDeath(FinPos, -1);
			GameServer()->CreateExplosion(FinPos, m_Owner, m_Weapon, false);
		}

		else if(TargetChr)
			TargetChr->TakeDamage(m_Direction * max(0.001f, m_Force), m_Damage, m_Owner, m_Weapon);

		GameServer()->m_World.DestroyEntity(this);
	}
}

void CLaserPlasma::TickPaused()
{
	++m_StartTick;
}

void CLaserPlasma::Snap(int SnappingClient)
{
	if(NetworkClipped(SnappingClient))
		return;

	CNetObj_Laser *pObj = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(NETOBJTYPE_LASER, m_ID, sizeof(CNetObj_Laser)));
	if(!pObj)
		return;

    int lngthX = (int)(m_Direction.x*m_Length);
    int lngthY = (int)(m_Direction.y*m_Length);
	pObj->m_X = (int)m_Pos.x;
	pObj->m_Y = (int)m_Pos.y;
	pObj->m_FromX = (int)m_Pos.x-lngthX;
	pObj->m_FromY = (int)m_Pos.y-lngthY;
	pObj->m_StartTick = Server()->Tick();
}
