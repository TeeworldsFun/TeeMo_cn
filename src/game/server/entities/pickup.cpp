/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "pickup.h"

CPickup::CPickup(CGameWorld *pGameWorld, int Type, int SubType)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_PICKUP)
{
	m_Type = Type;
	m_Subtype = SubType;
	m_ProximityRadius = PickupPhysSize;

	// TeeMo
    m_ToCID = -1;
    m_Vel = 0.0f;

	Reset();

	GameWorld()->InsertEntity(this);
}

void CPickup::Reset()
{
    // TeeMo
	if (m_Type != POWERUP_WEAPON_TEEMO && m_Type != POWERUP_INDICATOR_ARMOR && m_Type != POWERUP_INDICATOR_HEALTH && m_Type != POWERUP_HEALTH_M && g_pData->m_aPickups[m_Type].m_Spawndelay > 0)
		m_SpawnTick = Server()->Tick() + Server()->TickSpeed() * g_pData->m_aPickups[m_Type].m_Spawndelay;
	else
		m_SpawnTick = Server()->Tick();
}

void CPickup::Tick()
{
	// wait for respawn
	if(m_SpawnTick > 0 && m_Type != POWERUP_WEAPON_TEEMO && m_Type != POWERUP_INDICATOR_ARMOR && m_Type != POWERUP_INDICATOR_HEALTH && m_Type != POWERUP_HEALTH_M)
	{
		if(Server()->Tick() > m_SpawnTick)
		{
			// respawn
			m_SpawnTick = -1;
			if(m_Type == POWERUP_WEAPON)
				GameServer()->CreateSound(m_Pos, SOUND_WEAPON_SPAWN);
		}
		else
			return;
	}

	// TeeMo
    if ((m_Type == POWERUP_WEAPON_TEEMO && Server()->Tick()-m_SpawnTick > 800)
        || (m_Type == POWERUP_HEALTH_M && Server()->Tick()-m_SpawnTick > 2500))
    {
        GameServer()->m_World.DestroyEntity(this);
        return;
    }
    if (m_ToCID != -1)
    {
        if (!GameServer()->m_apPlayers[m_ToCID] || !GameServer()->m_apPlayers[m_ToCID]->GetCharacter())
        {
            GameServer()->m_World.DestroyEntity(this);
            return;
        }

        vec2 ToDir = normalize(GameServer()->m_apPlayers[m_ToCID]->GetCharacter()->m_Pos - m_Pos);
        m_Pos += ToDir*m_Vel;
        m_Vel+=0.25f;
    }

	// Check if a player intersected us
	CCharacter *pChr = GameServer()->m_World.ClosestCharacter(m_Pos, 20.0f, 0);
	if(pChr && pChr->IsAlive())
	{
		// player picked us up, is someone was hooking us, let them go
		int RespawnTime = -1;
		switch (m_Type)
		{
			case POWERUP_HEALTH:
				if(pChr->IncreaseHealth(1))
				{
					GameServer()->CreateSound(m_Pos, SOUND_PICKUP_HEALTH);
					RespawnTime = g_pData->m_aPickups[m_Type].m_Respawntime;
				}
				break;

			case POWERUP_HEALTH_M:
				if (GameServer()->m_apPlayers[m_ToCID] && GameServer()->m_apPlayers[m_ToCID]->GetCharacter() && GameServer()->m_apPlayers[m_ToCID]->GetCharacter() == pChr)
                {
                    pChr->IncreaseHealth(1);

                    GameServer()->CreateSound(m_Pos, SOUND_PICKUP_HEALTH);
                    GameServer()->m_World.DestroyEntity(this);
                    return;
                }
                break;

			case POWERUP_ARMOR:
				if(pChr->IncreaseArmor(1))
				{
					GameServer()->CreateSound(m_Pos, SOUND_PICKUP_ARMOR);
					RespawnTime = g_pData->m_aPickups[m_Type].m_Respawntime;
				}
				break;

            case POWERUP_WEAPON_TEEMO:
				if(m_Subtype >= 0 && m_Subtype < NUM_WEAPONS)
				{
					if(pChr->GiveWeapon(m_Subtype, 10))
					{
                        GameServer()->CreateSound(m_Pos, SOUND_PICKUP_GRENADE);
                        char buff[128];
                        if (m_Subtype == WEAPON_PHANTOM_GRENADE) { str_format(buff, sizeof(buff), "你捡到了一把幻影榴弹枪！"); }
                        else if (m_Subtype == WEAPON_AIR_GRENADE) { str_format(buff, sizeof(buff), "你捡到了一把空气榴弹炮！"); }
                        else if (m_Subtype == WEAPON_LASER_GRENADE) { str_format(buff, sizeof(buff), "你捡到了激光炸弹！"); }
                        else if (m_Subtype == WEAPON_PLASMA_RIFLE) { str_format(buff, sizeof(buff), "你捡到了一把等离子体发射器！"); }
                        else if (m_Subtype == WEAPON_DRAGON_GRENADE) { str_format(buff, sizeof(buff), "你捡到了一把龙炮！"); }
                        else { str_format(buff, sizeof(buff), "You give a UNKOWN WEAPON!"); }
                        GameServer()->SendChatTarget(pChr->GetPlayer()->GetCID(), buff);
                        GameServer()->m_World.DestroyEntity(this);
                        return;
					}
				}
				break;

			case POWERUP_WEAPON:
				if(m_Subtype >= 0 && m_Subtype < NUM_WEAPONS)
				{
					if(pChr->GiveWeapon(m_Subtype, 10))
					{
						RespawnTime = g_pData->m_aPickups[m_Type].m_Respawntime;

						if(m_Subtype == WEAPON_GRENADE)
							GameServer()->CreateSound(m_Pos, SOUND_PICKUP_GRENADE);
						else if(m_Subtype == WEAPON_SHOTGUN)
							GameServer()->CreateSound(m_Pos, SOUND_PICKUP_SHOTGUN);
						else if(m_Subtype == WEAPON_RIFLE)
							GameServer()->CreateSound(m_Pos, SOUND_PICKUP_SHOTGUN);

						if(pChr->GetPlayer())
							GameServer()->SendWeaponPickup(pChr->GetPlayer()->GetCID(), m_Subtype);
					}
				}
				break;

			case POWERUP_NINJA:
				{
					// activate ninja on target player
					pChr->GiveNinja();
					RespawnTime = g_pData->m_aPickups[m_Type].m_Respawntime;

					// loop through all players, setting their emotes
					CCharacter *pC = static_cast<CCharacter *>(GameServer()->m_World.FindFirst(CGameWorld::ENTTYPE_CHARACTER));
					for(; pC; pC = (CCharacter *)pC->TypeNext())
					{
						if (pC != pChr)
							pC->SetEmote(EMOTE_SURPRISE, Server()->Tick() + Server()->TickSpeed());
					}

					pChr->SetEmote(EMOTE_ANGRY, Server()->Tick() + 1200 * Server()->TickSpeed() / 1000);
					break;
				}

			default:
				break;
		};

		if(RespawnTime >= 0)
		{
			char aBuf[256];
			str_format(aBuf, sizeof(aBuf), "pickup player='%d:%s' item=%d/%d",
				pChr->GetPlayer()->GetCID(), Server()->ClientName(pChr->GetPlayer()->GetCID()), m_Type, m_Subtype);
			GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "game", aBuf);
			m_SpawnTick = Server()->Tick() + Server()->TickSpeed() * RespawnTime;
		}
	}
}

void CPickup::TickPaused()
{
	if(m_SpawnTick != -1)
		++m_SpawnTick;
}

void CPickup::Snap(int SnappingClient)
{
    // TeeMo
	if((m_SpawnTick != -1 && m_Type != POWERUP_WEAPON_TEEMO && m_Type != POWERUP_INDICATOR_ARMOR && m_Type != POWERUP_INDICATOR_HEALTH && m_Type != POWERUP_HEALTH_M) || NetworkClipped(SnappingClient))
		return;

    if ((m_Type == POWERUP_INDICATOR_HEALTH && GameServer()->m_apPlayers[SnappingClient]->GetInfoTEEMO()->GetRole() != ROLE_MEDIC) ||
        (m_Type == POWERUP_INDICATOR_ARMOR && GameServer()->m_apPlayers[SnappingClient]->GetInfoTEEMO()->GetRole() != ROLE_ENGINEER))
        return;

	CNetObj_Pickup *pP = static_cast<CNetObj_Pickup *>(Server()->SnapNewItem(NETOBJTYPE_PICKUP, m_ID, sizeof(CNetObj_Pickup)));
	if(!pP)
		return;

	pP->m_X = (int)m_Pos.x;
	pP->m_Y = (int)m_Pos.y;
	pP->m_Type = m_Type;

	// TeeMo
	if (m_Type == POWERUP_WEAPON_TEEMO)
    {
        pP->m_Type = POWERUP_WEAPON;
    }
    else if (m_Type == POWERUP_INDICATOR_ARMOR)
    {
        pP->m_Type = POWERUP_ARMOR;
    }
    else if (m_Type == POWERUP_INDICATOR_HEALTH ||
             m_Type == POWERUP_HEALTH_M)
    {
        pP->m_Type = POWERUP_HEALTH;
    }
    if (m_Subtype == WEAPON_PHANTOM_GRENADE ||
        m_Subtype == WEAPON_AIR_GRENADE ||
        m_Subtype == WEAPON_LASER_GRENADE ||
        m_Subtype == WEAPON_DRAGON_GRENADE ||
        m_Subtype == WEAPON_EXTRA_BOMB_GRENADE)
    {
        pP->m_Subtype = WEAPON_GRENADE;
    }
    else if (m_Subtype == WEAPON_PLASMA_RIFLE ||
             m_Subtype == WEAPON_HEALER_RIFLE ||
             m_Subtype == WEAPON_REPAIR_RIFLE)
    {
        pP->m_Subtype = WEAPON_RIFLE;
    }
    else if (m_Subtype == WEAPON_TURRET)
    {
        pP->m_Subtype = WEAPON_HAMMER;
    }
    else { pP->m_Subtype = m_Subtype; }
}
