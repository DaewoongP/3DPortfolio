#pragma once
#include "Task.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CTask_ChaseLook final : public CTask
{
private:
	explicit CTask_ChaseLook() = default;
	virtual ~CTask_ChaseLook() = default;

public:
	virtual HRESULT Initialize(CBlackBoard* pBlackBoard) override;
	virtual CBehavior::STATE Tick(_double dTimeDelta) override;
	virtual HRESULT Reset() override;

private:
	CTransform*		m_pTransformCom = { nullptr };
	_bool*			m_isWalk = { nullptr };

public:
	static CTask_ChaseLook* Create(class CBlackBoard* pBlackBoard);
	virtual void Free() override;
};

END