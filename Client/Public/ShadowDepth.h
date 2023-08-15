#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CShadowDepth final : public CGameObject
{
public:
	typedef struct tagShadowDepth
	{
		_float4x4		LightViewMatrix;
		_float4x4		LightProjMatrix;
		_float3			vCenter;
		_float3			vExtents;
		_float			fLightFar;
	}SHADOWDEPTH;

private:
	explicit CShadowDepth(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CShadowDepth(const CShadowDepth& rhs);
	virtual ~CShadowDepth() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;

private:
	_bool	IsInBoundingBox();

private:
	SHADOWDEPTH			m_DepthDesc;

public:
	static CShadowDepth* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END