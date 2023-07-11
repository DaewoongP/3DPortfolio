#pragma once
#include "Task.h"
#include "Client_Defines.h"

BEGIN(Client)

class CTask_RandomWait final : public CTask
{
private:
	explicit CTask_RandomWait() = default;
	virtual ~CTask_RandomWait() = default;

public:
	virtual HRESULT Initialize(CBlackBoard* pBlackBoard) override;
	virtual CBehavior::STATE Tick(_double dTimeDelta) override;
	virtual HRESULT Reset() override;

private:
	_uint*			m_iMaxWaitTime = { nullptr };
	_bool*			m_isWait = { nullptr };

public:
	static CTask_RandomWait* Create(class CBlackBoard* pBlackBoard);
	virtual void Free() override;
};

END