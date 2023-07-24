#pragma once
#include "Task.h"
#include "Client_Defines.h"

BEGIN(Client)
class CEnemy_Pistol;
class CEnemy_Hammer;
class CEnemy_Sword;

class CTask_Summon final : public CTask
{
private:
	explicit CTask_Summon() = default;
	virtual ~CTask_Summon() = default;

public:
	virtual HRESULT Initialize(CBlackBoard* pBlackBoard) override;
	virtual CBehavior::STATE Tick(_double dTimeDelta) override;
	virtual HRESULT Reset() override;

private:
	_float3		m_vCenter;
	_uint		m_iObjectIndex = { 0 };

private:
	CEnemy_Pistol*			m_pPistol = { nullptr };
	CEnemy_Hammer*			m_pHammer = { nullptr };
	CEnemy_Sword*			m_pSword = { nullptr };

private:
	HRESULT Summon_Pistol();
	HRESULT Summon_Hammer();
	HRESULT Summon_Sword();

public:
	static CTask_Summon* Create(class CBlackBoard* pBlackBoard);
	virtual void Free() override;
};

END