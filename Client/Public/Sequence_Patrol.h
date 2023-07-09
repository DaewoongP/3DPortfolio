#pragma once
#include "Sequence.h"

BEGIN(Engine)

class CSequence_Patrol final : public CSequence
{
private:
	explicit CSequence_Patrol() = default;
	virtual ~CSequence_Patrol() = default;

public:
	virtual HRESULT Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator) override;

public:
	static CSequence_Patrol* Create(CBlackBoard* pBlackBoard, CDecorator* pDecorator = nullptr);
	virtual void Free() override;
};

END