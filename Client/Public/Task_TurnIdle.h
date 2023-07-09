#pragma once
#include "Task.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CTask_TurnIdle final : public CTask
{
private:
	explicit CTask_TurnIdle() = default;
	virtual ~CTask_TurnIdle() = default;

public:
	virtual HRESULT Initialize(CBlackBoard * pBlackBoard) override;
	virtual CBehavior::STATE Tick(_double dTimeDelta) override;
	virtual HRESULT Reset() override;

private:
	CTransform* m_pTransformCom = { nullptr };
	_bool* m_isTurnLeft = { nullptr };

public:
	static CTask_TurnIdle* Create(class CBlackBoard* pBlackBoard);
	virtual void Free() override;
};

END