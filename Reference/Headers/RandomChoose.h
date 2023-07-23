#pragma once
#include "Behavior.h"

BEGIN(Engine)
class ENGINE_DLL CRandomChoose abstract : public CBehavior
{
protected:
	explicit CRandomChoose();
	virtual ~CRandomChoose() = default;

public:
	virtual HRESULT Initialize(CBlackBoard * pBlackBoard, CDecorator * pDecorator) override;
	virtual CBehavior::STATE Tick(_double dTimeDelta) override;

protected:
	vector<_float>		m_ChildWeights;

public:
	virtual void Free() override;
};

END