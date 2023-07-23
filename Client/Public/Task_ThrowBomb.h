#pragma once
#include "Task.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CTask_ThrowBomb final : public CTask
{
private:
	explicit CTask_ThrowBomb() = default;
	virtual ~CTask_ThrowBomb() = default;

public:
	virtual HRESULT Initialize(CBlackBoard * pBlackBoard) override;
	virtual CBehavior::STATE Tick(_double dTimeDelta) override;
	virtual HRESULT Reset() override;

private:
	CGameObject*		m_pOwner = { nullptr };

public:
	static CTask_ThrowBomb* Create(class CBlackBoard* pBlackBoard);
	virtual void Free() override;
};

END