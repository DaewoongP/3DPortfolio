#pragma once
#include "Selector.h"
#include "Client_Defines.h"

BEGIN(Client)

class CSelector_Hammer final : public CSelector
{
private:
	explicit CSelector_Hammer() = default;
	virtual ~CSelector_Hammer() = default;

public:
	virtual HRESULT Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator) override;

public:
	static CSelector_Hammer* Create(CBlackBoard* pBlackBoard, CDecorator* pDecorator = nullptr);
	virtual void Free() override;
};

END