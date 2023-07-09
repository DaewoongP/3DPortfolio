#pragma once
#include "Task.h"
#include "Client_Defines.h"

BEGIN(Client)

class CTask_Stop final : public CTask
{
private:
	explicit CTask_Stop() = default;
	virtual ~CTask_Stop() = default;

public:
	virtual HRESULT Initialize(CBlackBoard * pBlackBoard) override;
	virtual CBehavior::STATE Tick(_double dTimeDelta) override;
	virtual HRESULT Reset() override;

private:
	_bool*		m_isStop = { nullptr };

public:
	static CTask_Stop* Create(class CBlackBoard* pBlackBoard);
	virtual void Free() override;
};

END