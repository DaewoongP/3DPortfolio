#pragma once
#include "Sequence.h"

BEGIN(Engine)

class CSequence_Landing final : public CSequence
{
private:
	explicit CSequence_Landing() = default;
	virtual ~CSequence_Landing() = default;

public:
	virtual HRESULT Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator) override;

public:
	static CSequence_Landing* Create(CBlackBoard* pBlackBoard, CDecorator* pDecorator = nullptr);
	virtual void Free() override;
};

END