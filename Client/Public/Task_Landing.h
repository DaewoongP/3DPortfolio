#pragma once
#include "Task.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CTask_Landing final : public CTask
{
private:
	explicit CTask_Landing() = default;
	virtual ~CTask_Landing() = default;

public:
	virtual HRESULT Initialize(CBlackBoard * pBlackBoard) override;
	virtual CBehavior::STATE Tick(_double dTimeDelta) override;
	virtual HRESULT Reset() override;

private:
	CTransform*			m_pTransformCom = { nullptr };
	_double*			m_dLandingSpeed = { nullptr };
	_bool*				m_isFly = { nullptr };

private:
	_float4				m_vMapCenterPosition;

public:
	static CTask_Landing* Create(class CBlackBoard* pBlackBoard);
	virtual void Free() override;
};

END