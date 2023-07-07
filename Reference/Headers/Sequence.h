#pragma once
#include "Behavior.h"

BEGIN(Engine)

class ENGINE_DLL CSequence abstract : public CBehavior
{
protected:
	explicit CSequence();
	virtual ~CSequence() = default;

public:
	virtual HRESULT Initialize() PURE;
	virtual CBehavior::STATE Tick(class CBlackBoard* pBlackBoard, _double dTimeDelta) override;

public:
	virtual void Free() override;
};

END