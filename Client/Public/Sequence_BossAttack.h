#pragma once
#include "Sequence.h"
#include "Client_Defines.h"

BEGIN(Client)

class CSequence_BossAttack final : public CSequence
{
private:
	explicit CSequence_BossAttack() = default;
	virtual ~CSequence_BossAttack() = default;

public:
	virtual HRESULT Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator) override;

public:
	static CSequence_BossAttack* Create(CBlackBoard* pBlackBoard, CDecorator* pDecorator = nullptr);
	virtual void Free() override;
};

END