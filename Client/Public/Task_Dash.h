#pragma once
#include "Task.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
class CNavigation;
END

BEGIN(Client)

class CTask_Dash final : public CTask
{
private:
	explicit CTask_Dash() = default;
	virtual ~CTask_Dash() = default;

public:
	virtual HRESULT Initialize(CBlackBoard* pBlackBoard) override;
	virtual CBehavior::STATE Tick(_double dTimeDelta) override;
	virtual HRESULT Reset() override;

private:
	CTransform*			m_pTransformCom = { nullptr };
	CNavigation*		m_pNavigationCom = { nullptr };
	_bool*				m_isDash = { nullptr };
	_double*			m_dDashTime = { nullptr };
	_double*			m_dDashSpeed = { nullptr };
	_bool*				m_isBlocked = { nullptr };

private:
	_double				m_BlockedTime = { 0.0 };
	_double				m_dInitDashTime = { 0.0 };

public:
	static CTask_Dash* Create(class CBlackBoard* pBlackBoard);
	virtual void Free() override;
};

END