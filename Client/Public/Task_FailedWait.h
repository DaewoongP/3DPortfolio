#pragma once
#include "Task.h"
#include "Client_Defines.h"

BEGIN(Client)

class CTask_FailedWait final : public CTask
{
private:
	explicit CTask_FailedWait() = default;
	virtual ~CTask_FailedWait() = default;

public:
	virtual HRESULT Initialize(CBlackBoard* pBlackBoard) override;
	virtual CBehavior::STATE Tick(_double dTimeDelta) override;
	virtual HRESULT Reset() override;

private:
	_double* m_dWaitTime = { nullptr };
	_bool* m_isWait = { nullptr };

public:
	static CTask_FailedWait* Create(class CBlackBoard* pBlackBoard);
	virtual void Free() override;
};

END