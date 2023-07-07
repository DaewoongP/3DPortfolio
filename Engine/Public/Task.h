#pragma once
#include "Behavior.h"

BEGIN(Engine)

class ENGINE_DLL CTask abstract : public CBehavior
{
protected:
	explicit CTask();
	virtual ~CTask() = default;

public:
	virtual HRESULT Initialize(CBlackBoard* pBlackBoard) override;
	virtual CBehavior::STATE Tick(_double dTimeDelta) PURE;

public:
	virtual void Free() override;
};

END