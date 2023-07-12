#pragma once
#include "Task.h"
#include "Client_Defines.h"

BEGIN(Client)

class CTask_Ready final : public CTask
{
private:
	explicit CTask_Ready() = default;
	virtual ~CTask_Ready() = default;

public:
	virtual HRESULT Initialize(CBlackBoard* pBlackBoard) override;
	virtual CBehavior::STATE Tick(_double dTimeDelta) override;
	virtual HRESULT Reset() override;

private:
	_double*	m_dReadyTime = { nullptr };
	_bool*		m_isReady = { nullptr };

public:
	static CTask_Ready* Create(class CBlackBoard* pBlackBoard);
	virtual void Free() override;
};

END