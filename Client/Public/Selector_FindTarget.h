#pragma once
#include "Selector.h"
#include "Client_Defines.h"

BEGIN(Client)

class CSelector_FindTarget final : public CSelector
{
private:
	explicit CSelector_FindTarget() = default;
	virtual ~CSelector_FindTarget() = default;

public:
	virtual HRESULT Initialize(CBlackBoard * pBlackBoard, CDecorator * pDecorator) override;

public:
	static CSelector_FindTarget* Create(CBlackBoard * pBlackBoard, CDecorator * pDecorator = nullptr);
	virtual void Free() override;
};

END