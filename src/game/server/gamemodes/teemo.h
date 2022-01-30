/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_GAMEMODES_TEEMO_H
#define GAME_SERVER_GAMEMODES_TEEMO_H
#include <game/server/gamecontroller.h>
#include <game/server/entity.h>
#include <list>

#define TEEMO_VERSION   "3.7"
#define TEEMO_BUILD     "2022.01.30."

class CGameControllerTEEMO : public IGameController
{
public:
	class CFlag *m_apFlags[2];

	CGameControllerTEEMO(class CGameContext *pGameServer);
	virtual void DoWincheck();
	virtual bool CanBeMovedOnBalance(int ClientID);
	virtual void Snap(int SnappingClient);
	virtual void Tick();

    virtual bool OnChat(int ChatterClientID, int Team, const char *pText);
    virtual void OnClientEnter(int ClientID);
	virtual bool OnEntity(int Index, vec2 Pos);
	virtual void OnCharacterSpawn(class CCharacter *pChr);
	virtual int OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon);
	virtual void OnPlayerInfoChange(class CPlayer *pP);
	virtual const char *GetTeamName(int Team);
	virtual void OnEndRound();

	void UpdateClientClan(int ClientID);
	const char* GetRoleName(int role);
	int GetNumPlayersByRole(int team, int role);
	bool ChangeCharacterRole(int ClientID, const char *role);
	int GetRoleID(const char *role);
	bool HaveFlag(int cid);

	std::list<vec2> GetProtectedZones() { return m_ProtectedZones; }

protected:
    bool HaveOriginalConfig();

private:
    std::list<vec2> m_ProtectedZones;
};

#endif
