#pragma once
#include "Task.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CTask_BossBlock final : public CTask
{
private:
	explicit CTask_BossBlock() = default;
	virtual ~CTask_BossBlock() = default;

public:
	virtual HRESULT Initialize(CBlackBoard* pBlackBoard) override;
	virtual CBehavior::STATE Tick(_double dTimeDelta) override;
	virtual HRESULT Reset() override;

private:
	_bool* m_isBlock = { nullptr };

public:
	static CTask_BossBlock* Create(class CBlackBoard* pBlackBoard);
	virtual void Free() override;
};

END