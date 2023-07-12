#pragma once
#include "Selector.h"
#include "Client_Defines.h"

BEGIN(Client)

class CSelector_FindTargetToDashAttack final : public CSelector
{
private:
	explicit CSelector_FindTargetToDashAttack() = default;
	virtual ~CSelector_FindTargetToDashAttack() = default;

public:
	virtual HRESULT Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator) override;

public:
	static CSelector_FindTargetToDashAttack* Create(CBlackBoard* pBlackBoard, CDecorator* pDecorator = nullptr);
	virtual void Free() override;
};

END