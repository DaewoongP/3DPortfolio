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
	_vector Get_TransformState(CTransform::STATE eState) { return m_pTransform->Get_State(eState); }
	_matrix Get_CameraWorldMatrix() const { return m_pTransform->Get_WorldMatrix(); }
	_float	Get_FOV() const { return m_fFovy; }
	class CTransform* Get_Transform() { return m_pTransform; }
	void	Set_TransformDesc(CTransform::TRANSFORMDESC TransformDesc) { m_pTransform->Set_Desc(TransformDesc); }
	void	Set_CameraDesc(CAMERADESC CameraDesc);
	void	Set_CameraWorldMatrix(_fmatrix CamWorldMatrix) { m_pTransform->Set_WorldMatrix(CamWorldMatrix); }
	void	Set_LookAtLH(_float4 vEye, _float4 vAt, _float4 vUp = _float4(0.f, 1.f, 0.f, 0.f));
	void	Set_FOV(_float fFov) { m_fFovy = fFov; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick_Camera(_double dTimeDelta);

public:
	void Go_Straight(_double dTimeDelta) { m_pTransform->Go_Straight(dTimeDelta); }
	void Go_Backward(_double dTimeDelta) { m_pTransform->Go_Backward(dTimeDelta); }
	void Go_Left(_double dTimeDelta) { m_pTransform->Go_Left(dTimeDelta); }
	void Go_Right(_double dTimeDelta) { m_pTransform->Go_Right(dTimeDelta); }

	void Turn(_float fRoll, _float fPitch, _float fYaw) { m_pTransform->Turn(fRoll, fPitch, fYaw); }
	void Turn(_fvector vAxis, _double dTimeDelta) { m_pTransform->Turn(vAxis, dTimeDelta); }
	void Turn(_fvector vAxis, _float fRadian, _double dTimeDelta) { m_pTransform->Turn(vAxis, fRadian, dTimeDelta); }
	void Rotation(_fvector vAxis, _float fRadian, _bool bUseCurrentRotation = false) { m_pTransform->Rotation(vAxis, fRadian, bUseCurrentRotation); }
	void Set_Position(_fvector vPosition) { m_pTransform->Set_State(CTransform::STATE_POSITION, vPosition); }
	// 1. Roll 축 회전 힘, 2. Pitch 축 회전 힘, 3. Yaw 축 회전 힘, 4. ShakeTime : 총 회전시킬 시간, 5. ShakeTimeDelta : 이 값의 시간마다 회전, 6. Decrease : 감소시키며 흔들것인지, 7. DecreasePower : 얼마나 감소시킬것인지
	void Shake_RollPitchYaw(_float fRollPower, _float fPitchPower, _float fYawPower, _double dShakeTime, _double dShakeTimeDelta = 0.1, _bool isDecrease = true, _float fDecreasePower = 0.5f);

protected:
	class CTransform*	m_pTransform = { nullptr };
	class CPipeLine*	m_pPipeLine = { nullptr };
	_float4				m_vEye, m_vAt, m_vUp;
	_float				m_fFovy, m_fAspect, m_fNear, m_fFar;

protected:
	_float3				m_vRowPitchYaw;
	_double				m_dShakeTime = { 0.0 };
	_double				m_dShakeTimeAcc = { 0.0 };
	_bool				m_isDecreaseShakePower = { false };
	_float				m_fDecreasePower = { 0.f };
	_bool				m_isShaking = { false };
	// 흔드는 방향을 왼쪽 오른쪽 반복하기 위한 변수
	_bool				m_bShakeDir = { false };
	_double				m_dShakeTimeDelta = { 0.0 };

private:
	void Tick_Shake(_double dTimeDelta);

public:
	static CCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END