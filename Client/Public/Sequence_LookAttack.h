#pragma once
#include "Sequence.h"

BEGIN(Engine)

class CSequence_LookAttack final : public CSequence
{
private:
	explicit CSequence_LookAttack() = default;
	virtual ~CSequence_LookAttack() = default;

public:
	virtual HRESULT Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator) override;

public:
	static CSequence_LookAttack* Create(CBlackBoard* pBlackBoard, CDecorator* pDecorator = nullptr);
	virtual void Free() override;
};

END