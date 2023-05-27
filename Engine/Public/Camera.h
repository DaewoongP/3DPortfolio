#pragma once
#include "Composite.h"
#include "Transform.h"
BEGIN(Engine)
class CPipeLine;

class ENGINE_DLL CCamera final : public CComposite
{
public:
	typedef struct tagCameraDesc
	{
		_float4			vEye, vAt, vUp;
		_float			fFovy, fAspect, fNear, fFar;
		CTransform::TRANSFORMDESC		TransformDesc;
	}CAMERADESC;
private:
	explicit CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Tick(_double dTimeDelta) override;

protected:
	CTransform* m_pTransform = { nullptr };

	CPipeLine* m_pPipeLine = { nullptr };
	_float4						m_vEye, m_vAt, m_vUp;
	_float						m_fFovy = { 0 };
	_float						m_fAspect = { 0 };
	_float						m_fNear = { 0 };
	_float						m_fFar = { 0 };

public:
	static CCamera* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END