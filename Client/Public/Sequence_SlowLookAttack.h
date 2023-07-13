#pragma once
#include "Sequence.h"

BEGIN(Engine)

class CSequence_SlowLookAttack final : public CSequence
{
private:
	explicit CSequence_SlowLookAttack() = default;
	virtual ~CSequence_SlowLookAttack() = default;

public:
	virtual HRESULT Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator) override;

public:
	static CSequence_SlowLookAttack* Create(CBlackBoard* pBlackBoard, CDecorator* pDecorator = nullptr);
	virtual void Free() override;
};

END