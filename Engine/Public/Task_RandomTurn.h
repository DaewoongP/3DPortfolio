#pragma once
#include "Task.h"

BEGIN(Engine)

class ENGINE_DLL CTask_RandomTurn final : public CTask
{
private:
	explicit CTask_RandomTurn() = default;
	virtual ~CTask_RandomTurn() = default;

public:
	virtual HRESULT Initialize(CBlackBoard * pBlackBoard) override;
	virtual CBehavior::STATE Tick(_double dTimeDelta) override;

private:
	class CTransform* m_pOwnerTransformCom = { nullptr };

public:
	static CTask_RandomTurn* Create(class CBlackBoard* pBlackBoard);
	virtual void Free() override;
};

END