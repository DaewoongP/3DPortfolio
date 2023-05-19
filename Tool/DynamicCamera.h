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
	_float3		m_vEye;
	_float3		m_vAt;
	_float3		m_vUp;
	_float		m_fFov;
	_float		m_fAspect;
	_float		m_fNear;
	_float		m_fFar;
	_float4x4	m_matView;
	_float4x4	m_matProj;
	_bool		m_bFix;
	_bool		m_bClick;
	_float		m_fSpeed;

public:
	static CDynamicCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END