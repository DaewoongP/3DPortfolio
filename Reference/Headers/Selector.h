#pragma once
#include "Behavior.h"

BEGIN(Engine)

class ENGINE_DLL CSelector abstract : public CBehavior
{
protected:
	explicit CSelector();
	virtual ~CSelector() = default;

public:
	virtual HRESULT Initialize(CBlackBoard* pBlackBoard, CDecorator * pDecorator) override;
	virtual CBehavior::STATE Tick(_double dTimeDelta) override;

public:
	virtual void Free() override;
};

END