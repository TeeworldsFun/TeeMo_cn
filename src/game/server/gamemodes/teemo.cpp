/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <engine/shared/config.h>

#include <game/mapitems.h>

#include <game/server/entities/character.h>
#include <game/server/entities/flag.h>
#include <game/server/player.h>
#include <game/server/gamecontext.h>
#include <string>
#include "teemo.h"

CGameControllerTEEMO::CGameControllerTEEMO(class CGameContext *pGameServer)
: IGameController(pGameServer)
{
	m_apFlags[0] = 0;
	m_apFlags[1] = 0;
	m_pGameType = "TeeMo";
	m_GameFlags = GAMEFLAG_TEAMS|GAMEFLAG_FLAGS;
}

bool CGameControllerTEEMO::OnEntity(int Index, vec2 Pos)
{
	if(Index == ENTITY_FLAGSTAND_RED)
    {
        CFlag *F = new CFlag(&GameServer()->m_World, TEAM_RED);
        F->m_StandPos = Pos;
        F->m_Pos = Pos;
        m_apFlags[TEAM_RED] = F;
        GameServer()->m_World.InsertEntity(F);

         m_ProtectedZones.push_back(Pos);

        return true;
    } else if(Index == ENTITY_FLAGSTAND_BLUE)
    {
        CFlag *F = new CFlag(&GameServer()->m_World, TEAM_BLUE);
        F->m_StandPos = Pos;
        F->m_Pos = Pos;
        m_apFlags[TEAM_BLUE] = F;
        GameServer()->m_World.InsertEntity(F);

        m_ProtectedZones.push_back(Pos);

        return true;
    } else if(Index == ENTITY_SPAWN)
		 m_ProtectedZones.push_back(Pos);
	else if(Index == ENTITY_SPAWN_RED)
		 m_ProtectedZones.push_back(Pos);
	else if(Index == ENTITY_SPAWN_BLUE)
		 m_ProtectedZones.push_back(Pos);
    else if (Index == ENTITY_HEALTH_1)
        return false;
    //else if (Index == ENTITY_WEAPON_GRENADE || Index == ENTITY_WEAPON_SHOTGUN || Index == ENTITY_WEAPON_RIFLE)
    //    return false;

	if(IGameController::OnEntity(Index, Pos))
		return true;

	return false;
}

bool CGameControllerTEEMO::OnChat(int ChatterClientID, int Team, const char *pText)
{
    if (pText[0] != '/')
        return IGameController::OnChat(ChatterClientID, Team, pText);

    CPlayer *pPlayer = GameServer()->m_apPlayers[ChatterClientID];
    std::string message(pText);

    std::string command = message.substr(1);
    std::string param;
    if (message.find_first_of(32) != std::string::npos && message.find_first_of(32) != message.length()-1)
    {
        command = message.substr(1, message.find_first_of(32)-1);
        param = message.substr(message.find_first_of(32)+1);
    }

    if (str_comp_nocase(command.c_str(), "cmdlist") == 0)
    {
        GameServer()->SendChatTarget(ChatterClientID, "-");
        GameServer()->SendChatTarget(ChatterClientID, "TeeMo - 指令列表");
        GameServer()->SendChatTarget(ChatterClientID, "---------------- ---- -- -");
        GameServer()->SendChatTarget(ChatterClientID, "+ /role list    > 显示角色列表");
        GameServer()->SendChatTarget(ChatterClientID, "+ /role [角色名]  > 选择一个角色");
        GameServer()->SendChatTarget(ChatterClientID, "+ /about        > 关于TeeMo,教程");
    }
    else if (str_comp_nocase(command.c_str(), "role") == 0)
    {
        if (param.empty())
        {
            GameServer()->SendChatTarget(ChatterClientID, "无效的'/role'指令! 请输入/cmdlist获取帮助");
        }
        else
        {
            if (str_comp_nocase(param.c_str(), "list") == 0)
            {
                GameServer()->SendChatTarget(ChatterClientID, "-");
                GameServer()->SendChatTarget(ChatterClientID, "TeeMo - 角色列表");
                GameServer()->SendChatTarget(ChatterClientID, "---------- ---- -- -");
                GameServer()->SendChatTarget(ChatterClientID, "+ Soldier 士兵  --输入 '/role sol'成为此角色");
                GameServer()->SendChatTarget(ChatterClientID, "+ General 将军  --输入 '/role gen'成为此角色");
                GameServer()->SendChatTarget(ChatterClientID, "+ Medic 医生    --输入 '/role medic'成为此角色");
                GameServer()->SendChatTarget(ChatterClientID, "+ Spy 间谍      --输入 '/role spy'成为此角色");
                GameServer()->SendChatTarget(ChatterClientID, "+ Engineer 工程师-输入 '/role eng'成为此角色");
            }
            else if (str_comp_nocase(param.c_str(), "sol") == 0 ||
                       str_comp_nocase(param.c_str(), "gen") == 0 ||
                       str_comp_nocase(param.c_str(), "medic") == 0 ||
                       str_comp_nocase(param.c_str(), "spy") == 0 ||
                       str_comp_nocase(param.c_str(), "eng") == 0)
            {
                if (pPlayer->GetInfoTEEMO()->GetRole() == GetRoleID(param.c_str()))
                {
                    char buff[128];
                    str_format(buff, sizeof(buff), "你已经是%s了", GetRoleName(GetRoleID(param.c_str())));
                    GameServer()->SendChatTarget(ChatterClientID, buff);
                    return false;
                }

                if (ChangeCharacterRole(ChatterClientID, param.c_str()))
                {
                    if (str_comp_nocase(param.c_str(), "spy") != 0)
                    {
                        char buff[128];
                        str_format(buff, sizeof(buff), "'%s'将角色改为%s", Server()->ClientName(ChatterClientID), GetRoleName(GetRoleID(param.c_str())));
                        GameServer()->SendChat(-1, CGameContext::CHAT_ALL, buff);
                    }
                }
                else {
                    char buff[128];
                    str_format(buff, sizeof(buff), "无法将角色改为%s", GetRoleName(GetRoleID(param.c_str())));
                    GameServer()->SendChatTarget(ChatterClientID, buff);
                }
            }
            else
            {
                char buff[128];
                str_format(buff, sizeof(buff), "无效的参数'%s'", param.c_str());
                GameServer()->SendChatTarget(ChatterClientID, buff);
            }
        }
    }
    else if (str_comp_nocase(command.c_str(), "about") == 0)
    {
        GameServer()->SendChatTarget(ChatterClientID, "-");
        GameServer()->SendChatTarget(ChatterClientID, "TeeMo - 关于");
        GameServer()->SendChatTarget(ChatterClientID, "---------- ---- -- -");
        GameServer()->SendChatTarget(ChatterClientID, "作者: unsigned char* (teemo@redneboa.es)");
        GameServer()->SendChatTarget(ChatterClientID, "版本: v"TEEMO_VERSION" (build: "TEEMO_BUILD")");
        GameServer()->SendChatTarget(ChatterClientID, "-");
        GameServer()->SendChatTarget(ChatterClientID, "特别感谢");
        GameServer()->SendChatTarget(ChatterClientID, "----------------- ---- -- -");
        GameServer()->SendChatTarget(ChatterClientID, "MuffinMario (Beta-Tester)");
	GameServer()->SendChatTarget(ChatterClientID, "FlowerFell-Sans (中文翻译者,服务器托管)");
	GameServer()->SendChatTarget(ChatterClientID, "------------------------------------");
	GameServer()->SendChatTarget(ChatterClientID, "TeeMo - 教程");
	GameServer()->SendChatTarget(ChatterClientID, "将和你颜色不一样的人打死");
	GameServer()->SendChatTarget(ChatterClientID, "夺取和你颜色不一样的旗帜并将旗插到自己家旗");
	GameServer()->SendChatTarget(ChatterClientID, "多种武器类型，连续杀死敌人会获得奖励:");
	GameServer()->SendChatTarget(ChatterClientID, "1,飞行 2,隐身 3,爆炸钩 4,额外爆炸武器 5,短暂无敌");
	GameServer()->SendChatTarget(ChatterClientID, "服主QQ1421709710");
	GameServer()->SendChatTarget(ChatterClientID, "按F1查看全文");
    }
    else
    {
        char buff[128];
        str_format(buff, sizeof(buff), "无效的指令 '%s'", command.c_str());
        GameServer()->SendChatTarget(ChatterClientID, buff);
    }

    return false;
}

void CGameControllerTEEMO::OnClientEnter(int ClientID)
{
    UpdateClientClan(ClientID);
    GameServer()->m_apPlayers[ClientID]->GetInfoTEEMO()->Reset();
    GameServer()->m_apPlayers[ClientID]->GetInfoTEEMO()->SetRealTeam(GameServer()->m_apPlayers[ClientID]->GetTeam());

    GameServer()->SendChatTarget(ClientID, "欢迎来到TeeMo v"TEEMO_VERSION);
    GameServer()->SendChatTarget(ClientID, "输入/cmdlist获取角色指令");
    GameServer()->SendChatTarget(ClientID, " ");
    if (!HaveOriginalConfig())
    {
        GameServer()->SendChatTarget(ClientID, "-- 此服务器使用了tune");
        GameServer()->SendChatTarget(ClientID, " ");
    }


}

void CGameControllerTEEMO::OnPlayerInfoChange(class CPlayer *pP)
{
    if (pP->GetTeam() == TEAM_SPECTATORS) { pP->GetInfoTEEMO()->SetRole(ROLE_SOLDIER); }
    if (pP->GetInfoTEEMO()->GetRole() != ROLE_SPY) { pP->GetInfoTEEMO()->SetRealTeam(pP->GetTeam()); }
    UpdateClientClan(pP->GetCID());
}

bool CGameControllerTEEMO::ChangeCharacterRole(int ClientID, const char *role)
{
    CPlayer *pPlayer = GameServer()->m_apPlayers[ClientID];
    if (!pPlayer) { return false; }

    int roleId = GetRoleID(role);
    if (pPlayer->GetInfoTEEMO()->GetRole() == roleId || pPlayer->GetInfoTEEMO()->GetRole() == ROLE_SPY || (roleId == ROLE_SPY && HaveFlag(ClientID))) { return false; }
    int numPR = GetNumPlayersByRole(pPlayer->GetTeam(), roleId);

    if ((roleId == ROLE_GENERAL && GameServer()->Tuning()->m_MaxGenerals != -1 && numPR >= GameServer()->Tuning()->m_MaxGenerals) ||
        (roleId == ROLE_ENGINEER && GameServer()->Tuning()->m_MaxEngineers != -1 && numPR >= GameServer()->Tuning()->m_MaxEngineers) ||
        (roleId == ROLE_MEDIC && GameServer()->Tuning()->m_MaxMedics != -1 && numPR >= GameServer()->Tuning()->m_MaxMedics) ||
        (roleId < 0 || roleId >= MAX_ROLES))
    {
        return false;
    }
    else if (roleId == ROLE_SPY)
    {
        if (GameServer()->Tuning()->m_MaxSpies != -1 && numPR >= GameServer()->Tuning()->m_MaxSpies) { return false; }
        pPlayer->GetInfoTEEMO()->SetRole(ROLE_SPY);
        pPlayer->GetInfoTEEMO()->SetRealTeam(pPlayer->GetTeam());
        pPlayer->SetForceTeam(pPlayer->GetTeam()==TEAM_BLUE?TEAM_RED:TEAM_BLUE);
        return true;
    }

    pPlayer->GetInfoTEEMO()->SetRole(roleId);
    UpdateClientClan(ClientID);
    pPlayer->KillCharacter(WEAPON_GAME);
    return true;
}

int CGameControllerTEEMO::GetRoleID(const char *role)
{
    if (str_comp_nocase(role, "gen") == 0) { return ROLE_GENERAL; }
    else if (str_comp_nocase(role, "eng") == 0) { return ROLE_ENGINEER; }
    else if (str_comp_nocase(role, "spy") == 0) { return ROLE_SPY; }
    else if (str_comp_nocase(role, "medic") == 0) { return ROLE_MEDIC; }
    else { return ROLE_SOLDIER; }
}

int CGameControllerTEEMO::GetNumPlayersByRole(int team, int role)
{
    int res = 0;
    for (int i=0; i<MAX_CLIENTS; i++)
    {
        CPlayer *pPlayer = GameServer()->m_apPlayers[i];
        if (!pPlayer) { continue; }

        if ((team == -1 || pPlayer->GetTeam() == team) && pPlayer->GetInfoTEEMO()->GetRole() == role) { res++; }
    }

    return res;
}

void CGameControllerTEEMO::UpdateClientClan(int ClientID)
{
    CPlayer *pPlayer = GameServer()->m_apPlayers[ClientID];
    if (!pPlayer)
        return;

    int role = pPlayer->GetInfoTEEMO()->GetRole();
    Server()->SetClientClan(ClientID, GetRoleName(role));
}

const char* CGameControllerTEEMO::GetRoleName(int role)
{
    if (role == ROLE_ENGINEER) { return "工程师"; }
    else if (role == ROLE_GENERAL) { return "指挥官"; }
    else if (role == ROLE_MEDIC) { return "医生"; }
    else if (role == ROLE_SOLDIER) { return "士兵"; }
    else if (role == ROLE_SPY) { return "士兵"; } // Hide real name

    return "Unknown";
}

bool CGameControllerTEEMO::HaveOriginalConfig()
{
    CTuningParams tuningParams;
    CTuningParams *pTuning = GameServer()->Tuning();

    return mem_comp(pTuning, &tuningParams, sizeof(CTuningParams)) == 0;
}

void CGameControllerTEEMO::OnCharacterSpawn(class CCharacter *pChr)
{
    IGameController::OnCharacterSpawn(pChr);

    switch (pChr->GetPlayer()->GetInfoTEEMO()->GetRole())
    {
        case ROLE_SOLDIER:
            pChr->GiveWeapon(WEAPON_AIR_GRENADE, 3);
            pChr->GiveWeapon(WEAPON_DRAGON_GRENADE, 3);
            pChr->GiveWeapon(WEAPON_PLASMA_RIFLE, 4);
            break;

        case ROLE_ENGINEER:
            pChr->GiveWeapon(WEAPON_TURRET, -1);
            pChr->GiveWeapon(WEAPON_REPAIR_RIFLE, 1);
            break;

        case ROLE_GENERAL:
            pChr->GiveWeapon(WEAPON_PHANTOM_GRENADE, 6);
            pChr->GiveWeapon(WEAPON_DRAGON_GRENADE, 3);
            pChr->GiveWeapon(WEAPON_LASER_GRENADE, 4);
            break;

        case ROLE_SPY:
            pChr->GiveWeapon(WEAPON_PLASMA_RIFLE, 4);
            break;

        case ROLE_MEDIC:
            pChr->GiveWeapon(WEAPON_HEALER_RIFLE, 1);
            break;
    }
}

void CGameControllerTEEMO::OnEndRound()
{
    for (int i=0; i<MAX_CLIENTS; i++)
    {
        CPlayer *pPlayer = GameServer()->m_apPlayers[i];
        if (!pPlayer)
            continue;

        if (pPlayer->GetInfoTEEMO()->GetRole() == ROLE_SPY)
        {
            pPlayer->SetForceTeam(pPlayer->GetInfoTEEMO()->GetRealTeam());
            pPlayer->GetInfoTEEMO()->SetRole(ROLE_SOLDIER);
            UpdateClientClan(pPlayer->GetCID());
        }
    }
}

int CGameControllerTEEMO::OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int WeaponID)
{
	IGameController::OnCharacterDeath(pVictim, pKiller, WeaponID);
	int HadFlag = 0;

	// drop flags
	for(int i = 0; i < 2; i++)
	{
		CFlag *F = m_apFlags[i];
		if(F && pKiller && pKiller->GetCharacter() && F->m_pCarryingCharacter == pKiller->GetCharacter())
			HadFlag |= 2;
		if(F && F->m_pCarryingCharacter == pVictim)
		{
			GameServer()->CreateSoundGlobal(SOUND_CTF_DROP);
			F->m_DropTick = Server()->Tick();
			F->m_pCarryingCharacter = 0;
			F->m_Vel = vec2(0,0);

			if(pKiller && pKiller->GetTeam() != pVictim->GetPlayer()->GetTeam())
				pKiller->m_Score++;

			HadFlag |= 1;
		}
	}

	CPlayer *pPlayerVictim = pVictim->GetPlayer();
	if (pPlayerVictim->GetInfoTEEMO()->GetRole() == ROLE_SPY)
    {
        pPlayerVictim->SetForceTeam(pPlayerVictim->GetInfoTEEMO()->GetRealTeam());
        pPlayerVictim->GetInfoTEEMO()->SetRole(ROLE_SOLDIER);
        UpdateClientClan(pPlayerVictim->GetCID());

        char buff[128];
        str_format(buff, sizeof(buff), "'%s' 找到并杀死了一个间谍！", Server()->ClientName(pKiller->GetCID()));
        GameServer()->SendChat(-1, pKiller->GetTeam(), buff);
    }

    pVictim->GetPlayer()->GetInfoTEEMO()->m_Deaths++;
    pVictim->GetPlayer()->GetInfoTEEMO()->m_Kills=0;
    pVictim->GetPlayer()->GetInfoTEEMO()->ClearBonus();
    pKiller->GetInfoTEEMO()->m_Kills++;

    if (pVictim->m_pIndicatorPickup)
    {
        GameServer()->m_World.DestroyEntity(pVictim->m_pIndicatorPickup);
        pVictim->m_pIndicatorPickup = 0x0;
    }

    //drop weapons
    if (pVictim->GetWeapon() != WEAPON_HAMMER && pVictim->GetWeapon() != WEAPON_GUN && pVictim->GetWeapon() != WEAPON_RIFLE && pVictim->GetWeapon() != WEAPON_SHOTGUN && pVictim->GetWeapon() != WEAPON_NINJA && pVictim->GetWeapon() != WEAPON_GRENADE &&
        pVictim->GetWeapon() != WEAPON_HEALER_RIFLE && pVictim->GetWeapon() != WEAPON_REPAIR_RIFLE && pVictim->GetWeapon() != WEAPON_EXTRA_BOMB_GRENADE && pVictim->GetWeapon() != WEAPON_TURRET)
    {
        CPickup *pPickup = new CPickup(&GameServer()->m_World, POWERUP_WEAPON_TEEMO, pVictim->GetWeapon());
        pPickup->m_Pos = pVictim->m_Pos;
    }

    // bonus
    if (pKiller->GetInfoTEEMO()->m_Kills%(int)GameServer()->Tuning()->m_FlyBonusKills == 0 && !pKiller->GetInfoTEEMO()->HaveBonus(BONUS_FLY))
    {
        pKiller->GetInfoTEEMO()->AddBonus(BONUS_FLY);
        pKiller->GetInfoTEEMO()->m_FlyBonusTimer = Server()->Tick();
        char buff[128];
        str_format(buff, sizeof(buff), "'%s'在不死亡的情况下连续击杀了%d个玩家 获得了奖励 飞行", Server()->ClientName(pKiller->GetCID()), pKiller->GetInfoTEEMO()->m_Kills);
        GameServer()->SendChatTarget(-1, buff);
    }
    if (pKiller->GetInfoTEEMO()->m_Kills%(int)GameServer()->Tuning()->m_InvisibleBonusKills == 0 && !pKiller->GetInfoTEEMO()->HaveBonus(BONUS_INVISIBLE))
    {
        pKiller->GetInfoTEEMO()->AddBonus(BONUS_INVISIBLE);
        pKiller->GetInfoTEEMO()->m_InvisibleBonusTimer = Server()->Tick();
        char buff[128];
        str_format(buff, sizeof(buff), "'%s'在不死亡的情况下连续击杀了%d个玩家 获得了奖励 隐身.", Server()->ClientName(pKiller->GetCID()), pKiller->GetInfoTEEMO()->m_Kills);
        GameServer()->SendChatTarget(-1, buff);
    }
    if (pKiller->GetInfoTEEMO()->m_Kills%(int)GameServer()->Tuning()->m_HookerBonusKills == 0 && !pKiller->GetInfoTEEMO()->HaveBonus(BONUS_HOOKER))
    {
        pKiller->GetInfoTEEMO()->AddBonus(BONUS_HOOKER);
        pKiller->GetInfoTEEMO()->m_HookerBonusTimer = Server()->Tick();
        char buff[128];
        str_format(buff, sizeof(buff), "'%s'在不死亡的情况下连续击杀了%d个玩家 获得了奖励buff 爆炸钩", Server()->ClientName(pKiller->GetCID()), pKiller->GetInfoTEEMO()->m_Kills);
        GameServer()->SendChatTarget(-1, buff);
    }
    if (pKiller->GetInfoTEEMO()->m_Kills%(int)GameServer()->Tuning()->m_ExtraBombBonusKills == 0 && pKiller->GetCharacter())
    {
        pKiller->GetCharacter()->GiveWeapon(WEAPON_EXTRA_BOMB_GRENADE, 10);
        char buff[128];
        str_format(buff, sizeof(buff), "'%s'在不死亡的情况下连续击杀了%d个玩家 获得了吉良吉影的第二炸弹！", pKiller->GetInfoTEEMO()->m_Kills);
        GameServer()->SendChatTarget(pKiller->GetCID(), buff);
    }
    if (pKiller->GetInfoTEEMO()->m_Kills%(int)GameServer()->Tuning()->m_GodBonusKills == 0 && !pKiller->GetInfoTEEMO()->HaveBonus(BONUS_GOD))
    {
        pKiller->GetInfoTEEMO()->AddBonus(BONUS_GOD);
        pKiller->GetInfoTEEMO()->m_GodBonusTimer = Server()->Tick();
        char buff[128];
        str_format(buff, sizeof(buff), "'%s'在不死亡的情况下连续击杀了%d个玩家 获得了奖励 无敌.", Server()->ClientName(pKiller->GetCID()), pKiller->GetInfoTEEMO()->m_Kills);
        GameServer()->SendChatTarget(-1, buff);
    }

    // drop health to killer
    if (pKiller != pVictim->GetPlayer())
    {
        CPickup *pPickup = new CPickup(&GameServer()->m_World, POWERUP_HEALTH_M);
        pPickup->m_Pos = pVictim->m_Pos;
        pPickup->m_ToCID = pKiller->GetCID();
    }

	return HadFlag;
}

void CGameControllerTEEMO::DoWincheck()
{
	if(m_GameOverTick == -1 && !m_Warmup)
	{
		// check score win condition
		if((g_Config.m_SvScorelimit > 0 && (m_aTeamscore[TEAM_RED] >= g_Config.m_SvScorelimit || m_aTeamscore[TEAM_BLUE] >= g_Config.m_SvScorelimit)) ||
			(g_Config.m_SvTimelimit > 0 && (Server()->Tick()-m_RoundStartTick) >= g_Config.m_SvTimelimit*Server()->TickSpeed()*60))
		{
			if(m_SuddenDeath)
			{
				if(m_aTeamscore[TEAM_RED]/100 != m_aTeamscore[TEAM_BLUE]/100)
					EndRound();
			}
			else
			{
				if(m_aTeamscore[TEAM_RED] != m_aTeamscore[TEAM_BLUE])
					EndRound();
				else
					m_SuddenDeath = 1;
			}
		}
	}
}

const char* CGameControllerTEEMO::GetTeamName(int Team)
{
    if(Team == TEAM_RED)
        return "人类";
    else if(Team == TEAM_BLUE)
        return "外星人";

    return IGameController::GetTeamName(Team);
}

bool CGameControllerTEEMO::HaveFlag(int cid)
{
    CCharacter *pChar = GameServer()->GetPlayerChar(cid);
    if (!pChar)
        return false;

    for (int i=0; i<2; i++)
    {
        CFlag *pFlag = m_apFlags[i];
        if (pFlag->m_pCarryingCharacter == pChar)
            return true;
    }

    return false;
}

bool CGameControllerTEEMO::CanBeMovedOnBalance(int ClientID)
{
	CCharacter* Character = GameServer()->m_apPlayers[ClientID]->GetCharacter();
	if(Character)
	{
		for(int fi = 0; fi < 2; fi++)
		{
			CFlag *F = m_apFlags[fi];
			if(F && F->m_pCarryingCharacter == Character)
				return false;
		}
	}

	if (GameServer()->m_apPlayers[ClientID]->GetInfoTEEMO()->GetRole() == ROLE_SPY)
        return false;

	return true;
}

void CGameControllerTEEMO::Snap(int SnappingClient)
{
	IGameController::Snap(SnappingClient);

	CNetObj_GameData *pGameDataObj = (CNetObj_GameData *)Server()->SnapNewItem(NETOBJTYPE_GAMEDATA, 0, sizeof(CNetObj_GameData));
	if(!pGameDataObj)
		return;

	pGameDataObj->m_TeamscoreRed = m_aTeamscore[TEAM_RED];
	pGameDataObj->m_TeamscoreBlue = m_aTeamscore[TEAM_BLUE];

	if(m_apFlags[TEAM_RED])
	{
		if(m_apFlags[TEAM_RED]->m_AtStand)
			pGameDataObj->m_FlagCarrierRed = FLAG_ATSTAND;
		else if(m_apFlags[TEAM_RED]->m_pCarryingCharacter && m_apFlags[TEAM_RED]->m_pCarryingCharacter->GetPlayer())
			pGameDataObj->m_FlagCarrierRed = m_apFlags[TEAM_RED]->m_pCarryingCharacter->GetPlayer()->GetCID();
		else
			pGameDataObj->m_FlagCarrierRed = FLAG_TAKEN;
	}
	else
		pGameDataObj->m_FlagCarrierRed = FLAG_MISSING;
	if(m_apFlags[TEAM_BLUE])
	{
		if(m_apFlags[TEAM_BLUE]->m_AtStand)
			pGameDataObj->m_FlagCarrierBlue = FLAG_ATSTAND;
		else if(m_apFlags[TEAM_BLUE]->m_pCarryingCharacter && m_apFlags[TEAM_BLUE]->m_pCarryingCharacter->GetPlayer())
			pGameDataObj->m_FlagCarrierBlue = m_apFlags[TEAM_BLUE]->m_pCarryingCharacter->GetPlayer()->GetCID();
		else
			pGameDataObj->m_FlagCarrierBlue = FLAG_TAKEN;
	}
	else
		pGameDataObj->m_FlagCarrierBlue = FLAG_MISSING;
}

void CGameControllerTEEMO::Tick()
{
	IGameController::Tick();

	if(GameServer()->m_World.m_ResetRequested || GameServer()->m_World.m_Paused)
		return;

	for(int fi = 0; fi < 2; fi++)
	{
		CFlag *F = m_apFlags[fi];

		if(!F)
			continue;

		// flag hits death-tile or left the game layer, reset it
		if((GameServer()->Collision()->GetCollisionAt(F->m_Pos.x, F->m_Pos.y)&CCollision::COLFLAG_DEATH) || F->GameLayerClipped(F->m_Pos))
		{
			GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "game", "flag_return");
			GameServer()->CreateSoundGlobal(SOUND_CTF_RETURN);
			F->Reset();
			continue;
		}

		//
		if(F->m_pCarryingCharacter)
		{
			// update flag position
			F->m_Pos = F->m_pCarryingCharacter->m_Pos;

			if(m_apFlags[fi^1] && m_apFlags[fi^1]->m_AtStand)
			{
				if(distance(F->m_Pos, m_apFlags[fi^1]->m_Pos) < CFlag::ms_PhysSize + CCharacter::ms_PhysSize)
				{
					// CAPTURE! \o/
					m_aTeamscore[fi^1] += 100;
					F->m_pCarryingCharacter->GetPlayer()->m_Score += 5;

					char aBuf[512];
					str_format(aBuf, sizeof(aBuf), "flag_capture player='%d:%s'",
						F->m_pCarryingCharacter->GetPlayer()->GetCID(),
						Server()->ClientName(F->m_pCarryingCharacter->GetPlayer()->GetCID()));
					GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "game", aBuf);

					float CaptureTime = (Server()->Tick() - F->m_GrabTick)/(float)Server()->TickSpeed();
					if(CaptureTime <= 60)
					{
						str_format(aBuf, sizeof(aBuf), "The %s flag was captured by '%s' (%d.%s%d seconds)", fi ? "blue" : "red", Server()->ClientName(F->m_pCarryingCharacter->GetPlayer()->GetCID()), (int)CaptureTime%60, ((int)(CaptureTime*100)%100)<10?"0":"", (int)(CaptureTime*100)%100);
					}
					else
					{
						str_format(aBuf, sizeof(aBuf), "The %s flag was captured by '%s'", fi ? "blue" : "red", Server()->ClientName(F->m_pCarryingCharacter->GetPlayer()->GetCID()));
					}
					GameServer()->SendChat(-1, -2, aBuf);
					for(int i = 0; i < 2; i++)
						m_apFlags[i]->Reset();

					GameServer()->CreateSoundGlobal(SOUND_CTF_CAPTURE);
				}
			}
		}
		else
		{
			CCharacter *apCloseCCharacters[MAX_CLIENTS];
			int Num = GameServer()->m_World.FindEntities(F->m_Pos, CFlag::ms_PhysSize, (CEntity**)apCloseCCharacters, MAX_CLIENTS, CGameWorld::ENTTYPE_CHARACTER);
			for(int i = 0; i < Num; i++)
			{
				if(!apCloseCCharacters[i]->IsAlive() || apCloseCCharacters[i]->GetPlayer()->GetTeam() == TEAM_SPECTATORS || GameServer()->Collision()->IntersectLine(F->m_Pos, apCloseCCharacters[i]->m_Pos, NULL, NULL)
                    || apCloseCCharacters[i]->GetPlayer()->GetInfoTEEMO()->GetRole() == ROLE_SPY)
					continue;

				if(apCloseCCharacters[i]->GetPlayer()->GetTeam() == F->m_Team)
				{
					// return the flag
					if(!F->m_AtStand)
					{
						CCharacter *pChr = apCloseCCharacters[i];
						pChr->GetPlayer()->m_Score += 1;

						char aBuf[256];
						str_format(aBuf, sizeof(aBuf), "flag_return player='%d:%s'",
							pChr->GetPlayer()->GetCID(),
							Server()->ClientName(pChr->GetPlayer()->GetCID()));
						GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "game", aBuf);

						GameServer()->CreateSoundGlobal(SOUND_CTF_RETURN);
						F->Reset();
					}
				}
				else
				{
					// take the flag
					if(F->m_AtStand)
					{
						m_aTeamscore[fi^1]++;
						F->m_GrabTick = Server()->Tick();
					}

					F->m_AtStand = 0;
					F->m_pCarryingCharacter = apCloseCCharacters[i];
					F->m_pCarryingCharacter->GetPlayer()->m_Score += 1;

					char aBuf[256];
					str_format(aBuf, sizeof(aBuf), "flag_grab player='%d:%s'",
						F->m_pCarryingCharacter->GetPlayer()->GetCID(),
						Server()->ClientName(F->m_pCarryingCharacter->GetPlayer()->GetCID()));
					GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "game", aBuf);

					for(int c = 0; c < MAX_CLIENTS; c++)
					{
						CPlayer *pPlayer = GameServer()->m_apPlayers[c];
						if(!pPlayer)
							continue;

						if(pPlayer->GetTeam() == TEAM_SPECTATORS && pPlayer->m_SpectatorID != SPEC_FREEVIEW && GameServer()->m_apPlayers[pPlayer->m_SpectatorID] && GameServer()->m_apPlayers[pPlayer->m_SpectatorID]->GetTeam() == fi)
							GameServer()->CreateSoundGlobal(SOUND_CTF_GRAB_EN, c);
						else if(pPlayer->GetTeam() == fi)
							GameServer()->CreateSoundGlobal(SOUND_CTF_GRAB_EN, c);
						else
							GameServer()->CreateSoundGlobal(SOUND_CTF_GRAB_PL, c);
					}
					// demo record entry
					GameServer()->CreateSoundGlobal(SOUND_CTF_GRAB_EN, -2);
					break;
				}
			}

			if(!F->m_pCarryingCharacter && !F->m_AtStand)
			{
				if(Server()->Tick() > F->m_DropTick + Server()->TickSpeed()*30)
				{
					GameServer()->CreateSoundGlobal(SOUND_CTF_RETURN);
					F->Reset();
				}
				else
				{
					F->m_Vel.y += GameServer()->m_World.m_Core.m_Tuning.m_Gravity;
					GameServer()->Collision()->MoveBox(&F->m_Pos, &F->m_Vel, vec2(F->ms_PhysSize, F->ms_PhysSize), 0.5f);
				}
			}
		}
	}

	for (int i=0; i<MAX_CLIENTS; i++)
    {
        CPlayer *pPlayer = GameServer()->m_apPlayers[i];
        if (!pPlayer || pPlayer->GetTeam() == TEAM_SPECTATORS)
            continue;

        // Remove Bonus
        if (pPlayer->GetInfoTEEMO()->HaveBonus(BONUS_FLY) && Server()->Tick()-pPlayer->GetInfoTEEMO()->m_FlyBonusTimer > GameServer()->Tuning()->m_FlyBonusLifetime/Server()->TickSpeed())
        {
            GameServer()->SendChatTarget(pPlayer->GetCID(), "飞行效果结束！哈哈哈");
            pPlayer->GetInfoTEEMO()->DelBonus(BONUS_FLY);
        }
        if (pPlayer->GetInfoTEEMO()->HaveBonus(BONUS_GOD) && Server()->Tick()-pPlayer->GetInfoTEEMO()->m_GodBonusTimer > GameServer()->Tuning()->m_GodBonusLifetime/Server()->TickSpeed())
        {
            GameServer()->SendChatTarget(pPlayer->GetCID(), "上帝效果结束！");
            pPlayer->GetInfoTEEMO()->DelBonus(BONUS_GOD);
        }
        if (pPlayer->GetInfoTEEMO()->HaveBonus(BONUS_INVISIBLE) && Server()->Tick()-pPlayer->GetInfoTEEMO()->m_InvisibleBonusTimer > GameServer()->Tuning()->m_InvisibleBonusLifetime/Server()->TickSpeed())
        {
            GameServer()->SendChatTarget(pPlayer->GetCID(), "隐身效果结束！");
            pPlayer->GetInfoTEEMO()->DelBonus(BONUS_INVISIBLE);
        }
        if (pPlayer->GetInfoTEEMO()->HaveBonus(BONUS_HOOKER) && Server()->Tick()-pPlayer->GetInfoTEEMO()->m_HookerBonusTimer > GameServer()->Tuning()->m_HookerBonusLifetime/Server()->TickSpeed())
        {
            GameServer()->SendChatTarget(pPlayer->GetCID(), "爆炸钩奖励结束！");
            pPlayer->GetInfoTEEMO()->DelBonus(BONUS_HOOKER);
        }

        // Indicators
        if (pPlayer->GetCharacter())
        {
            if (pPlayer->GetCharacter()->GetHealth() < 10 && !pPlayer->GetCharacter()->m_pIndicatorPickup)
                pPlayer->GetCharacter()->m_pIndicatorPickup = new CPickup(&GameServer()->m_World, POWERUP_INDICATOR_HEALTH);

            if (pPlayer->GetCharacter()->m_pIndicatorPickup)
            {
                pPlayer->GetCharacter()->m_pIndicatorPickup->m_Pos = pPlayer->GetCharacter()->m_Pos-vec2(0,50);

                if (pPlayer->GetCharacter()->GetHealth() >= 10)
                {
                    GameServer()->m_World.DestroyEntity(pPlayer->GetCharacter()->m_pIndicatorPickup);
                    pPlayer->GetCharacter()->m_pIndicatorPickup = 0x0;
                }
            }
        }
    }
}
