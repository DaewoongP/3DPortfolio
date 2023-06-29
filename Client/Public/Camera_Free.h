#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CCamera;
END

BEGIN(Client)

class CCamera_Free : public CGameObject
{
private:
	explicit CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCamera_Free(const CCamera_Free& rhs);
	virtual ~CCamera_Free() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;

private:
	HRESULT Add_Component();

private:
	void		Key_Input(const _double& dTimeDelta);
	void		Mouse_Move(_double dTimeDelta);
	void		Fix_Mouse(void);

private:
	// 마우스 고정 변수
	_bool		m_bFix = { true };

private:
	CCamera*		m_pCameraCom = { nullptr };

public:
	static CCamera_Free* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END