#pragma once
#include "RandomChoose.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CRandomChoose_Move final : public CRandomChoose
{
private:
	explicit CRandomChoose_Move() = default;
	virtual ~CRandomChoose_Move() = default;

public:
	virtual HRESULT Initialize(CBlackBoard* pBlackBoard, CDecorator* pDecorator) override;

public:
	static CRandomChoose_Move* Create(CBlackBoard* pBlackBoard, CDecorator* pDecorator = nullptr);
	virtual void Free() override;
};

END