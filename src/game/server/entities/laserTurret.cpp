/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "laserTurret.h"
#include "laserGrenade.h" // Laser without bouncing

CLaserTurret::CLaserTurret(CGameWorld *pGameWorld, vec2 Pos, int Owner)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_LASER)
{
	m_Pos = Pos;
	m_Owner = Owner;
	GameWorld()->InsertEntity(this);
	m_ShootTick = Server()->Tick();
	m_Health = GameServer()->Tuning()->m_LaserTurretHealth;
	m_DamageTakenTick = 0;
	m_DamageTaken = 0;
}

void CLaserTurret::SetHealth(int health)
{
    m_Health = health;
    m_Health = min(m_Health, (int)GameServer()->Tuning()->m_LaserTurretHealth);
}

bool CLaserTurret::HitCharacter()
{
    if (m_BuildProcess < 100.0f || Server()->Tick()-m_ShootTick < GameServer()->Tuning()->m_LaserTurretFireDelay/Server()->TickSpeed())
        return false;

    m_ShootTick = Server()->Tick();

    CCharacter *apEnts[MAX_CLIENTS];
    int Num = GameServer()->m_World.FindEntities(m_Pos, GameServer()->Tuning()->m_LaserTurretRadius, (CEntity**)apEnts, MAX_CLIENTS, CGameWorld::ENTTYPE_CHARACTER);
    for(int i = 0; i < Num; i++)
    {
        if (apEnts[i]->GetPlayer()->GetCID() == m_Owner || GameServer()->m_pController->IsFriendlyFire(apEnts[i]->GetPlayer()->GetCID(), m_Owner))
            continue;

        vec2 Diff = apEnts[i]->m_Pos - m_Pos;
        float dist = distance(apEnts[i]->m_Pos, m_Pos);
        if (dist < GameServer()->Tuning()->m_LaserTurretRadius && !GameServer()->Collision()->IntersectLine(m_Pos, apEnts[i]->m_Pos, 0x0, 0x0))
        {
            vec2 dir = apEnts[i]->m_Pos - m_Pos;
            dir = normalize(dir);
            new CLaserGrenade(GameWorld(), m_Pos, dir, GameServer()->Tuning()->m_LaserTurretRadius, m_Owner);
            return true;
        }
    }

    return false;
}

void CLaserTurret::TakeDamage(int damage, int pID)
{
    CPlayer *pPlayer = GameServer()->m_apPlayers[pID];
    if (!pPlayer)
        return;

    m_Health -= damage;
    m_Health = max(m_Health, 0);

	m_DamageTaken++;
	if(Server()->Tick() < m_DamageTakenTick+25)
	    GameServer()->CreateDamageInd(m_Pos, m_DamageTaken*0.25f, damage);
	else
	{
		m_DamageTaken = 0;
		GameServer()->CreateDamageInd(m_Pos, 0, damage);
	}
	m_DamageTakenTick = Server()->Tick();

    if (m_Health == 0.0f)
    {
        if (pID != m_Owner)
        {
            char buff[255];
            str_format(buff, sizeof(buff), "'%s' destroyed one of your turrets!", Server()->ClientName(pID));
            GameServer()->SendChatTarget(m_Owner, buff);
        }

        Reset();
    }
}

void CLaserTurret::Reset()
{
    if (m_pIndicatorPickup)
        GameServer()->m_World.DestroyEntity(m_pIndicatorPickup);

    GameServer()->CreateExplosion(m_Pos, m_Owner, WEAPON_TURRET, true);
	GameServer()->m_World.DestroyEntity(this);
}

void CLaserTurret::Tick()
{
    if (!GameServer()->m_apPlayers[m_Owner] || !GameServer()->m_apPlayers[m_Owner]->GetCharacter())
        Reset();

    if (m_Health < GameServer()->Tuning()->m_LaserTurretHealth && !m_pIndicatorPickup)
    {
        m_pIndicatorPickup = new CPickup(&GameServer()->m_World, POWERUP_INDICATOR_ARMOR);
        m_pIndicatorPickup->m_Pos = m_Pos-vec2(0,30);
    }

    if (m_pIndicatorPickup && m_Health >= GameServer()->Tuning()->m_LaserTurretHealth)
    {
        GameServer()->m_World.DestroyEntity(m_pIndicatorPickup);
        m_pIndicatorPickup = 0x0;
    }

	HitCharacter();
}

void CLaserTurret::Snap(int SnappingClient)
{
	if(NetworkClipped(SnappingClient))
		return;

	CNetObj_Laser *pObj = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(NETOBJTYPE_LASER, m_ID, sizeof(CNetObj_Laser)));
	if(!pObj)
		return;

	pObj->m_X = (int)m_Pos.x;
	pObj->m_Y = (int)m_Pos.y;
	pObj->m_FromX = (int)m_Pos.x;
	pObj->m_FromY = (int)m_Pos.y;
	pObj->m_StartTick = Server()->Tick();
}
