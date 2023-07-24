#pragma once
#include "Sequence.h"

BEGIN(Engine)

class CSequence_Charge final : public CSequence
{
private:
	explicit CSequence_Charge() = default;
	virtual ~CSequence_Charge() = default;

public:
	virtual HRESULT Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator) override;

public:
	static CSequence_Charge* Create(CBlackBoard* pBlackBoard, CDecorator* pDecorator = nullptr);
	virtual void Free() override;
};

END