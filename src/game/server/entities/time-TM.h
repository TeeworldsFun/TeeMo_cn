#ifndef GAME_SERVER_ENTITIES_TIME_H
#define GAME_SERVER_ENTITIES_TIME_H

#include <game/server/entity.h>

enum
{
	SEC = 0,
	MIN,
	HOUR
};

class CTime : public CEntity
{
	struct Hand
	{
		int m_Length;
		float m_Rotation;
		vec2 m_To;
	} m_Hand[3];

	int m_ID2;
	int m_ID3;
	int m_IDs[8];

	void SetHandRota();
	int GetSnapID(int Hand);

public:
	CTime(CGameWorld *pGameWorld, vec2 Pos);
	virtual ~CTime();

	virtual void Tick();
	virtual void Snap(int SnappingClient);
};

#endif // GAME_SERVER_ENTITIES_TIME_H