#pragma once
#include "Task.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CTask_LensFlare final : public CTask
{
private:
	explicit CTask_LensFlare() = default;
	virtual ~CTask_LensFlare() = default;

public:
	virtual HRESULT Initialize(CBlackBoard * pBlackBoard) override;
	virtual CBehavior::STATE Tick(_double dTimeDelta) override;
	virtual HRESULT Reset() override;

private:
	class CLensFlare*		m_pLensFlare = { nullptr };
	CTransform*				m_pTransformCom = { nullptr };
	_float4x4*				m_LensMatrix = { nullptr };

public:
	static CTask_LensFlare* Create(class CBlackBoard* pBlackBoard);
	virtual void Free() override;
};

END