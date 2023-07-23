#pragma once
#include "RandomChoose.h"
#include "Client_Defines.h"

BEGIN(Client)

class CRandomChoose_FlyingAttack final : public CRandomChoose
{
private:
	explicit CRandomChoose_FlyingAttack() = default;
	virtual ~CRandomChoose_FlyingAttack() = default;

public:
	virtual HRESULT Initialize(CBlackBoard * pBlackBoard, CDecorator * pDecorator) override;

public:
	static CRandomChoose_FlyingAttack* Create(CBlackBoard * pBlackBoard, CDecorator * pDecorator = nullptr);
	virtual void Free() override;
};

END