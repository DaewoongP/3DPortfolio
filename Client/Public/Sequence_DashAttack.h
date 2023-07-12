#pragma once
#include "Sequence.h"

BEGIN(Engine)

class CSequence_DashAttack final : public CSequence
{
private:
	explicit CSequence_DashAttack() = default;
	virtual ~CSequence_DashAttack() = default;

public:
	virtual HRESULT Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator) override;

public:
	static CSequence_DashAttack* Create(CBlackBoard* pBlackBoard, CDecorator* pDecorator = nullptr);
	virtual void Free() override;
};

END