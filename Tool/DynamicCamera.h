#pragma once
#include "GameObject.h"

BEGIN(Tool)

class CDynamicCamera final : public CGameObject
{
private:
	explicit CDynamicCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDynamicCamera(const CDynamicCamera& rhs);
	virtual ~CDynamicCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;

public:
	HRESULT		Add_Components();


private:
	void		Key_Input(const _double& dTimeDelta);
	void		Mouse_Move(void);
	void		Fix_Mouse(void);

private:
	_vector		m_vEye = { 0 };
	_vector		m_vAt = { 0 };
	_vector		m_vUp = { 0 };
	_float		m_fFov = { 0 };
	_float		m_fAspect = { 0 };
	_float		m_fNear = { 0 };
	_float		m_fFar = { 0 };
	_matrix		m_matView;
	_matrix		m_matProj;
	_bool		m_bFix = { 0 };
	_bool		m_bClick = { 0 };
	_float		m_fSpeed = { 0 };

public:
	static CDynamicCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END