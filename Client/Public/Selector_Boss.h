#pragma once
#include "Selector.h"
#include "Client_Defines.h"

BEGIN(Client)

class CSelector_Boss final : public CSelector
{
private:
	explicit CSelector_Boss() = default;
	virtual ~CSelector_Boss() = default;

public:
	virtual HRESULT Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator) override;

public:
	static CSelector_Boss* Create(CBlackBoard* pBlackBoard, CDecorator* pDecorator = nullptr);
	virtual void Free() override;
};

END