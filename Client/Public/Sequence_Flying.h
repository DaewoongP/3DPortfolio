#pragma once
#include "Sequence.h"

BEGIN(Engine)

class CSequence_Flying final : public CSequence
{
private:
	explicit CSequence_Flying() = default;
	virtual ~CSequence_Flying() = default;

public:
	virtual HRESULT Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator) override;

public:
	static CSequence_Flying* Create(CBlackBoard* pBlackBoard, CDecorator* pDecorator = nullptr);
	virtual void Free() override;
};

END