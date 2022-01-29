/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "laserHealer.h"

CLaserHealer::CLaserHealer(CGameWorld *pGameWorld, vec2 Pos, vec2 Direction, float StartEnergy, int Owner)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_LASER)
{
	m_Pos = Pos;
	m_Owner = Owner;
	m_Energy = StartEnergy;
	m_Dir = Direction;
	GameWorld()->InsertEntity(this);
	m_HealerTimer = Server()->Tick();
}


bool CLaserHealer::HitCharacter(vec2 From, vec2 To)
{
	vec2 At;
	CCharacter *pOwnerChar = GameServer()->GetPlayerChar(m_Owner);
	CCharacter *pHit = GameServer()->m_World.IntersectCharacter(To, m_Pos, 0.f, At, pOwnerChar);
	if(!pHit)
		return false;

	m_From = At;

	if (Server()->Tick()-m_HealerTimer > GameServer()->Tuning()->m_LaserHealerSpeed * Server()->TickSpeed() / 1000)
    {
        if (GameServer()->m_pController->IsFriendlyFire(pHit->GetPlayer()->GetCID(), m_Owner))
        {
            pHit->IncreaseHealth(1);
            pHit->SetEmote(EMOTE_HAPPY, Server()->Tick() + 500 * Server()->TickSpeed() / 1000);
        }
        else
            pHit->TakeDamage(vec2(0.f, 0.f), 1, m_Owner, WEAPON_RIFLE);

        char buff[128];
        str_format(buff, sizeof(buff), "玩家血量: %d", pHit->GetHealth());
        GameServer()->SendBroadcast(buff, m_Owner);
        m_HealerTimer = Server()->Tick();
    }

	return true;
}

void CLaserHealer::Reset()
{
	GameServer()->m_World.DestroyEntity(this);
}

void CLaserHealer::Tick()
{
    CCharacter *pOwnerChar = GameServer()->GetPlayerChar(m_Owner);
    if (!pOwnerChar) { Reset(); return; }

    m_Pos = pOwnerChar->m_Pos;
    m_Dir = pOwnerChar->GetDirection();
    vec2 To = m_Pos + m_Dir * m_Energy;
    int Collide = GameServer()->Collision()->IntersectLine(m_Pos, To, 0x0, &To);
    m_From = To;

    if (Collide)
        GameServer()->CreateSound(m_Pos, SOUND_RIFLE_BOUNCE);
    else
        HitCharacter(m_Pos, m_From);
}

void CLaserHealer::Snap(int SnappingClient)
{
	//if(NetworkClipped(SnappingClient))
	//	return;

	CNetObj_Laser *pObj = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(NETOBJTYPE_LASER, m_ID, sizeof(CNetObj_Laser)));
	if(!pObj)
		return;

	pObj->m_X = (int)m_From.x;
	pObj->m_Y = (int)m_From.y;
	pObj->m_FromX = (int)m_Pos.x;
	pObj->m_FromY = (int)m_Pos.y;
	pObj->m_StartTick = Server()->Tick();
}
