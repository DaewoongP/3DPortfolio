#pragma once
#include "Sequence.h"
#include "Client_Defines.h"

BEGIN(Client)

class CSequence_BossBlock final : public CSequence
{
private:
	explicit CSequence_BossBlock() = default;
	virtual ~CSequence_BossBlock() = default;

public:
	virtual HRESULT Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator) override;

public:
	static CSequence_BossBlock* Create(CBlackBoard* pBlackBoard, CDecorator* pDecorator = nullptr);
	virtual void Free() override;
};

END