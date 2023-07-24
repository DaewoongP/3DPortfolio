#pragma once
#include "RandomChoose.h"
#include "Client_Defines.h"

BEGIN(Client)

class CRandomChoose_AttackSequence final : public CRandomChoose
{
private:
	explicit CRandomChoose_AttackSequence() = default;
	virtual ~CRandomChoose_AttackSequence() = default;

public:
	virtual HRESULT Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator) override;

public:
	static CRandomChoose_AttackSequence* Create(CBlackBoard* pBlackBoard, CDecorator* pDecorator = nullptr);
	virtual void Free() override;
};

END