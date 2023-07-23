#pragma once
#include "Task.h"
#include "Client_Defines.h"

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

public:
	static CTask_Fly* Create(class CBlackBoard* pBlackBoard);
	virtual void Free() override;
};

END