/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "character.h"
#include "projectileTEEMO.h"
#include "laserGrenade.h"
#include "laserTurret.h"

CProjectileLaserGrenade::CProjectileLaserGrenade(CGameWorld *pGameWorld, int Type, int Owner, vec2 Pos, vec2 Dir, int Span,
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
	m_Vel = m_Direction*(GameServer()->Tuning()->m_GrenadeSpeed/1000);
	m_DragonTime = Server()->Tick();
	m_DragonActive = false;

	GameWorld()->InsertEntity(this);
}

void CProjectileLaserGrenade::Reset()
{
    if (m_Type == WEAPON_AIR_GRENADE)
    {
        CPlayer *pPlayer = GameServer()->m_apPlayers[m_Owner];
        if (pPlayer)
        {
            CCharacter *pChar = pPlayer->GetCharacter();
            if (pChar) { pChar->GiveAmmo(m_Type, 1); }
        }
    }

	GameServer()->m_World.DestroyEntity(this);
}

vec2 CProjectileLaserGrenade::GetPos(float Time)
{
	float Curvature = 0;
	float Speed = 0;

	switch(m_Type)
	{
		case WEAPON_PHANTOM_GRENADE:
        case WEAPON_LASER_GRENADE:
        case WEAPON_DRAGON_GRENADE:
			Curvature = GameServer()->Tuning()->m_GrenadeCurvature;
			Speed = GameServer()->Tuning()->m_GrenadeSpeed;

			break;

        case WEAPON_AIR_GRENADE:
            Curvature = 0;
            Speed = 0;

            break;
        
        case WEAPON_EXTRA_BOMB_GRENADE:
            Curvature = 0;
            Speed = 900;

            break;
	}

	return CalcPos(m_Pos, m_Direction, Curvature, Speed, Time);
}


void CProjectileLaserGrenade::Tick()
{
    if (!GameServer()->m_apPlayers[m_Owner] || !GameServer()->m_apPlayers[m_Owner]->GetCharacter())
    {
        GameServer()->m_World.DestroyEntity(this);
        return;
    }

	float Pt = (Server()->Tick()-m_StartTick-1)/(float)Server()->TickSpeed();
	float Ct = (Server()->Tick()-m_StartTick)/(float)Server()->TickSpeed();
	vec2 PrevPos = GetPos(Pt);
	vec2 CurPos = GetPos(Ct);
	vec2 FinPos;

	int Collide = GameServer()->Collision()->IntersectLine(PrevPos, CurPos, &CurPos, &FinPos);
	CCharacter *OwnerChar = GameServer()->GetPlayerChar(m_Owner);
	CCharacter *TargetChr = GameServer()->m_World.IntersectCharacter(PrevPos, CurPos, 6.0f, CurPos, OwnerChar);

	m_LifeSpan--;

    if (m_Type == WEAPON_LASER_GRENADE && Collide)
    {
        vec2 tmpPos = FinPos;
        GameServer()->Collision()->MovePoint(&tmpPos, &m_Vel, 0.5f, 0x0);
        m_Direction = normalize(m_Vel);
        m_Pos = tmpPos;
        m_StartTick = Server()->Tick();
        Collide = 0;
    } else if (m_Type == WEAPON_PHANTOM_GRENADE)
    {
        Collide = 0;
    } else if (m_Type == WEAPON_DRAGON_GRENADE)
    {
        if ((!m_DragonActive && Server()->Tick()-m_DragonTime >= 15) || (m_DragonActive && Server()->Tick()-m_DragonTime >= GameServer()->Tuning()->m_DragonGrenadeExplosionDelay/Server()->TickSpeed()))
        {
            if (!m_DragonActive)
                m_DragonActive = true;

            GameServer()->CreateExplosion(CurPos, m_Owner, m_Weapon, false);
            GameServer()->CreateSound(CurPos, SOUND_HAMMER_FIRE);
            m_DragonTime = Server()->Tick();
        }
    } else if (m_Type == WEAPON_AIR_GRENADE && !Collide)
    {
        CCharacter *apEnts[MAX_CLIENTS];
        int Num = GameServer()->m_World.FindEntities(m_Pos, 14.0f, (CEntity**)apEnts, MAX_CLIENTS, CGameWorld::ENTTYPE_CHARACTER);
        int NumTeam = 0;
        for (int i = 0; i < Num; ++i)
        {
            CCharacter *pTarget = apEnts[i];
            if (!pTarget || pTarget->GetPlayer()->GetTeam() == GameServer()->m_apPlayers[m_Owner]->GetTeam())
                continue;

            NumTeam++;
        }

        if (NumTeam > 0)
            Collide = true;
    } else if (m_Type == WEAPON_EXTRA_BOMB_GRENADE)
    {
        for(int i = 0; i < MAX_CLIENTS; i++)
        {
            CCharacter* pHit = GameServer()->GetPlayerChar(i);

            if(!pHit)
                continue;

            if(distance(pHit->m_Pos, m_Pos) > 60)
                continue;

            if(Server()->Tick()%25 == 0)
                continue;

            if(pHit->GetPlayer()->GetTeam() == GameServer()->m_apPlayers[m_Owner]->GetTeam())
            {
                if(pHit->GetHealth() < 10)
                    pHit->IncreaseHealth(1);
                else
                    continue;
            }
            else
                pHit->TakeDamage(vec2(0, 0), 1, m_Owner, -1);
       }
    }

	if (!Collide)
    {
        CEntity *apEnts[1];
        int Num = GameServer()->m_World.FindEntities(CurPos, 28.0f, (CEntity**)apEnts, 1, CGameWorld::ENTTYPE_LASER);
        if (Num > 0)
        {
            CLaserTurret *pLaserTurret = dynamic_cast<CLaserTurret*>(apEnts[0]);
            if (pLaserTurret)
            {
                pLaserTurret->TakeDamage(m_Damage, m_Owner);
                if (pLaserTurret->GetHealth() > 0.0f)
                {
                    char buff[255]={0};
                    str_format(buff, sizeof(buff), "炮塔血量: %d", pLaserTurret->GetHealth());
                    GameServer()->SendBroadcast(buff, m_Owner);
                }
                else
                    GameServer()->SendBroadcast("炮塔被摧毁！", m_Owner);

                Collide = true;
            }
        }
    }

	if(TargetChr || Collide || m_LifeSpan < 0 || GameLayerClipped(CurPos))
	{
		if(m_LifeSpan >= 0 || m_Weapon == WEAPON_GRENADE)
			GameServer()->CreateSound(CurPos, m_SoundImpact);

		if(m_Explosive)
			GameServer()->CreateExplosion(CurPos, m_Owner, m_Weapon, false);

		else if(TargetChr)
			TargetChr->TakeDamage(m_Direction * max(0.001f, m_Force), m_Damage, m_Owner, m_Weapon);

        if (!TargetChr && m_Type == WEAPON_LASER_GRENADE)
        {
            vec2 Spreading[10];
            Spreading[0] = vec2(-1,0);
            Spreading[1] = vec2(1,0);
            Spreading[2] = vec2(0,-1);
            Spreading[3] = vec2(0,1);
            Spreading[4] = vec2(-1,1);
            Spreading[5] = vec2(1,-1);
            Spreading[6] = vec2(1,1);
            Spreading[7] = vec2(-1,-1);
            Spreading[8] = vec2(0,0);
            Spreading[9] = GameServer()->m_apPlayers[m_Owner]->GetCharacter()->m_Pos;
			for(int i = 0; i <= sizeof(Spreading)/sizeof(vec2); ++i)
                new CLaserGrenade(GameWorld(), PrevPos, Spreading[i], i%2==0?GameServer()->Tuning()->m_LaserGrenadeReach:GameServer()->Tuning()->m_LaserGrenadeReach-(GameServer()->Tuning()->m_LaserGrenadeReach/2), m_Owner);

			GameServer()->CreateSound(CurPos, SOUND_RIFLE_FIRE);
        }

		Reset();
	}
}

void CProjectileLaserGrenade::TickPaused()
{
	++m_StartTick;
}

void CProjectileLaserGrenade::FillInfo(CNetObj_Projectile *pProj)
{
    pProj->m_StartTick = m_StartTick;
	pProj->m_X = (int)m_Pos.x;
	pProj->m_Y = (int)m_Pos.y;

    if (m_Type == WEAPON_AIR_GRENADE)
    {
        pProj->m_VelX = 0;
        pProj->m_VelY = 0;
        pProj->m_StartTick = Server()->Tick()-2;
    }
    else
    {
    	pProj->m_VelX = (int)(m_Direction.x*100.0f);
        pProj->m_VelY = (int)(m_Direction.y*100.0f);
    }

	if (m_Type == WEAPON_PHANTOM_GRENADE ||
        m_Type == WEAPON_AIR_GRENADE ||
        m_Type == WEAPON_LASER_GRENADE ||
        m_Type == WEAPON_DRAGON_GRENADE)
    {
        pProj->m_Type = WEAPON_GRENADE;
    }
    else { pProj->m_Type = m_Type; }
}

void CProjectileLaserGrenade::Snap(int SnappingClient)
{
	float Ct = (Server()->Tick()-m_StartTick)/(float)Server()->TickSpeed();

	if(NetworkClipped(SnappingClient, GetPos(Ct)))
		return;

	CNetObj_Projectile *pProj = static_cast<CNetObj_Projectile *>(Server()->SnapNewItem(NETOBJTYPE_PROJECTILE, m_ID, sizeof(CNetObj_Projectile)));
	if(pProj)
		FillInfo(pProj);
}
