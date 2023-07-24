#pragma once
#include "Task.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CTask_Charge final : public CTask
{
private:
	explicit CTask_Charge() = default;
	virtual ~CTask_Charge() = default;

public:
	virtual HRESULT Initialize(CBlackBoard* pBlackBoard) override;
	virtual CBehavior::STATE Tick(_double dTimeDelta) override;
	virtual HRESULT Reset() override;

private:
	CTransform*		m_pTransformCom = { nullptr };
	_double*		m_dChargeReadyTime = { nullptr };
	_double*		m_dChargeSpeed = { nullptr };
	_bool*			m_isCharge = { nullptr };

private:
	_float			m_fStopLength = { 0.f };

public:
	static CTask_Charge* Create(class CBlackBoard* pBlackBoard);
	virtual void Free() override;
};

END