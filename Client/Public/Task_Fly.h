#pragma once
#include "Task.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CTask_Fly final : public CTask
{
private:
	explicit CTask_Fly() = default;
	virtual ~CTask_Fly() = default;

public:
	virtual HRESULT Initialize(CBlackBoard * pBlackBoard) override;
	virtual CBehavior::STATE Tick(_double dTimeDelta) override;
	virtual HRESULT Reset() override;

private:
	CTransform*			m_pTransformCom = { nullptr };
	_float*				m_fFlyHeight = { nullptr };
	_float*				m_fFlySpeed = { nullptr };

public:
	static CTask_Fly* Create(class CBlackBoard* pBlackBoard);
	virtual void Free() override;
};

END