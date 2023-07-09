#pragma once
#include "Task.h"
#include "Client_Defines.h"

BEGIN(Client)

class CTask_Attack final : public CTask
{
private:
	explicit CTask_Attack() = default;
	virtual ~CTask_Attack() = default;

public:
	virtual HRESULT Initialize(CBlackBoard * pBlackBoard) override;
	virtual CBehavior::STATE Tick(_double dTimeDelta) override;
	virtual HRESULT Reset() override;

private:
	_double*				m_dAttackCooltime = { nullptr };

public:
	static CTask_Attack* Create(class CBlackBoard* pBlackBoard);
	virtual void Free() override;
};

END