#pragma once
#include "Task.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CTask_RandomTurn final : public CTask
{
private:
	explicit CTask_RandomTurn() = default;
	virtual ~CTask_RandomTurn() = default;

public:
	virtual HRESULT Initialize(CBlackBoard * pBlackBoard) override;
	virtual CBehavior::STATE Tick(_double dTimeDelta) override;
	virtual HRESULT Reset() override;

private:
	CTransform*			m_pTransformCom = { nullptr };
	_double*			m_dTurnTime = { nullptr };
	_bool*				m_isTurn = { nullptr };
	_bool*				m_isTurnLeft = { nullptr };

private:
	_bool				m_bRand = { false };
	_float				m_fRandRad = { 0.f };

public:
	static CTask_RandomTurn* Create(class CBlackBoard* pBlackBoard);
	virtual void Free() override;
};

END