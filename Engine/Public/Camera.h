#pragma once
#include "Composite.h"
#include "Transform.h"

BEGIN(Engine)

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
	void Set_CameraDesc(CAMERADESC CameraDesc);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;

public:
	void Go_Straight(_double dTimeDelta) { m_pTransform->Go_Straight(dTimeDelta); }
	void Go_Backward(_double dTimeDelta) { m_pTransform->Go_Backward(dTimeDelta); }
	void Go_Left(_double dTimeDelta) { m_pTransform->Go_Left(dTimeDelta); }
	void Go_Right(_double dTimeDelta) { m_pTransform->Go_Right(dTimeDelta); }

	_vector Get_TransformState(CTransform::STATE eState) { return m_pTransform->Get_State(eState); }
	void Set_TransformDesc(CTransform::TRANSFORMDESC TransformDesc) { m_pTransform->Set_Desc(TransformDesc); }
	void Turn(_fvector vAxis, _double dTimeDelta) { m_pTransform->Turn(vAxis, dTimeDelta); }

protected:
	class CTransform*	m_pTransform = { nullptr };
	class CPipeLine*	m_pPipeLine = { nullptr };
	_float4				m_vEye, m_vAt, m_vUp;
	_float				m_fFovy, m_fAspect, m_fNear, m_fFar;

public:
	static CCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END