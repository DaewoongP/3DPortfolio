#pragma once
#include "Behavior.h"

BEGIN(Engine)

class ENGINE_DLL CTask abstract : public CBehavior
{
protected:
	explicit CTask();
	virtual ~CTask() = default;

public:
	virtual HRESULT Initialize() PURE;
	virtual CBehavior::STATE Tick(class CBlackBoard* pBlackBoard, _double dTimeDelta) PURE;

public:
	virtual void Free() override;
};

END