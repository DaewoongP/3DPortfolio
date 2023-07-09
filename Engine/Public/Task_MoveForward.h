#pragma once
#include "Task.h"

BEGIN(Engine)

class ENGINE_DLL CTask_MoveForward final : public CTask
{
private:
	explicit CTask_MoveForward() = default;
	virtual ~CTask_MoveForward() = default;

public:
	virtual HRESULT Initialize(CBlackBoard* pBlackBoard) override;
	virtual CBehavior::STATE Tick(_double dTimeDelta) override;

private:
	class CTransform*	m_pOwnerTransformCom = { nullptr };

public:
	static CTask_MoveForward* Create(class CBlackBoard* pBlackBoard);
	virtual void Free() override;
};

END