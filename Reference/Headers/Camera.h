#pragma once
#include "Composite.h"

BEGIN(Engine)

class ENGINE_DLL CCamera final : public CComposite
{
public:
	enum class PROJECTION { PROJ_FOV, PROJ_ASPECT, PROJ_NEAR, PROJ_FAR, PROJ_END };
public:
	typedef struct tagCameraDesc
	{
		_float4		vEye;
		_float4		vFocus;
		_float4		vUp;
		_float4		vProjection;

	}CAMERADESC;
private:
	explicit CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	_vector Get_ViewState();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

private:
	_float4x4			m_ViewMatrix;
	_float4x4			m_ProjectionMatrix;

	CAMERADESC			m_CameraDesc;

private:
	CComposite*			m_pComposite = { nullptr };

public:
	static CCamera* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END