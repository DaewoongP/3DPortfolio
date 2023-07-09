#pragma once
#include "Task.h"
#include "Client_Defines.h"

BEGIN(Client)

class CTask_Wait final : public CTask
{
private:
	explicit CTask_Wait() = default;
	virtual ~CTask_Wait() = default;

public:
	virtual HRESULT Initialize(CBlackBoard* pBlackBoard) override;
	virtual CBehavior::STATE Tick(_double dTimeDelta) override;
	virtual HRESULT Reset() override;

private:
	_double*		m_dWaitTime = { nullptr };
	_bool*			m_isWait = { nullptr };

public:
	static CTask_Wait* Create(class CBlackBoard* pBlackBoard);
	virtual void Free() override;
};

END