#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CCamera;
class CShader;
class CCollider;
class CRenderer;
class CTransform;
class CNavigation;
END

BEGIN(Client)

class CPlayer final : public CGameObject
{
public:
	enum STATE { STATE_IDLE, STATE_ATTACK, STATE_RUN, STATE_RUNWALL, STATE_CROUCH, STATE_JUMP, STATE_CLIMB, STATE_DRONRIDE, STATE_END };

private:
	explicit CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual void Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CModel*					m_pModelCom = { nullptr };
	CCamera*				m_pPlayerCameraCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	CCollider*				m_pColliderCom = { nullptr };
	CRenderer*				m_pRendererCom = { nullptr };
	CTransform*				m_pTransformCom = { nullptr };
	CNavigation*			m_pNavigation = { nullptr };

private:
	// 채널의 인덱스 값을 가지고 있음.
	_uint					m_iWeaponR = { 0 };
	// 마우스 감도
	_float					m_fMouseSensitivity = { 0.f };


#ifdef _DEBUG
	_bool					m_isMouseFixed = { true };
#endif // _DEBUG


private:
	HRESULT Add_Component();
	HRESULT SetUp_ShaderResources();
	HRESULT Find_BoneIndices();
	void Key_Input(_double dTimeDelta);
	void Fix_Mouse();
	// 1인칭 뷰의 카메라 오프셋값
	void CameraOffset();

private: /* Setup Files */
	HRESULT SetUp_AnimationNotifies(const _tchar* pNotifyFilePath);
	HRESULT SetUp_Collider(const _tchar* pColliderFilePath);

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END