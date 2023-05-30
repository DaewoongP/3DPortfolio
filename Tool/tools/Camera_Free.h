#pragma once
#include "GameObject.h"

BEGIN(Engine)
class CCamera;
END

BEGIN(Tool)

class CCamera_Free final : public CGameObject
{
private:
	explicit CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCamera_Free(const CCamera_Free& rhs);
	virtual ~CCamera_Free() = default;

public:
	void Set_Speed(_float fSpeed);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;

public:
	HRESULT		Add_Components();

private:
	void		Key_Input(const _double& dTimeDelta);
	void		Mouse_Move(_double dTimeDelta);
	void		Fix_Mouse(void);

public:
	_vector		Picking();

private:
	_bool		m_bFix = { 0 };
	_bool		m_bClick = { 0 };

private:
	CCamera*	m_pCamera = { nullptr };

public:
	static CCamera_Free* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END