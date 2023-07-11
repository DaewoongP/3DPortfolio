#pragma once
#include "Selector.h"
#include "Client_Defines.h"

BEGIN(Client)

class CSelector_FindTargetToAttack final : public CSelector
{
private:
	explicit CSelector_FindTargetToAttack() = default;
	virtual ~CSelector_FindTargetToAttack() = default;

public:
	virtual HRESULT Initialize(CBlackBoard * pBlackBoard, CDecorator * pDecorator) override;

public:
	static CSelector_FindTargetToAttack* Create(CBlackBoard * pBlackBoard, CDecorator * pDecorator = nullptr);
	virtual void Free() override;
};

END