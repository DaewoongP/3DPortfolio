#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CCamera;
class CShader;
class CCollider;
class CRenderer;
class CNavigation;
END

BEGIN(Client)

class CPlayer final : public CGameObject
{
public:
	enum STATE { STATE_IDLE, STATE_ATTACK, STATE_RUN, STATE_RUNWALL_L, STATE_RUNWALL_R, STATE_DASH, STATE_CROUCH, STATE_HOOK, STATE_CLIMB, STATE_DRONRIDE, STATE_DEAD, STATE_END };

private:
	explicit CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	STATE Get_CurrentState() const { return m_eCurState; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual void OnCollisionEnter(COLLISIONDESC CollisionDesc) override;
	virtual void OnCollisionStay(COLLISIONDESC CollisionDesc) override;
	virtual void OnCollisionExit(COLLISIONDESC CollisionDesc) override;
	virtual HRESULT Render() override;
	virtual HRESULT Reset() override;

private:
	CModel*					m_pModelCom = { nullptr };
	CCamera*				m_pPlayerCameraCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	CCollider*				m_pColliderCom = { nullptr };
	CRenderer*				m_pRendererCom = { nullptr };
	CNavigation*			m_pNavigationCom = { nullptr };

private: // 사실 이거 굳이 가지고 있을 필요는 없음. 이미 컴포넌트로 Tick을 돌고있음,
	class CKatana*			m_pKatana = { nullptr };
	CCollider*				m_pVisionColliderCom = { nullptr };

private:
	// 마우스 감도
	_float					m_fMouseSensitivity = { 0.f };
	// 현재 실행되고 있는 애니메이션 상태.
	ANIMATIONFLAG			m_eCurrentAnimationFlag = { ANIM_END };

	// 현재와 이전 상태를 비교해서 애니메이션 변경처리
	STATE					m_ePreState;
	STATE					m_eCurState;

	_bool					m_isCrouch = { false };

	_float					m_fSpeed;
	DASHDESC				m_Dash;

	_double					m_dAnimationLerpDuration = { 0.2 };

private: // WallRun
	_float					m_fWallRunY = { 0.f };
	_float					m_fWallRunAngle = { 0.f };
	_bool					m_isWallRotated = { false };
	_float3					m_vWallDir;
	_bool					m_isWallRun = { false };

private:
	vector<const CCollider*>	m_InRangeEnemyColliders;
	_float3					m_vAttackPositon;

private:
	_float4x4				m_CameraMatrix;

#ifdef _DEBUG
	_bool					m_isMouseFixed = { true };
	_bool					m_isInvisible = { false };
#endif // _DEBUG

private: /* Tick */
	HRESULT Add_Component();
	HRESULT Add_Parts();
	HRESULT SetUp_ShaderResources();
	HRESULT Initailize_Skills();

	void Key_Input(_double dTimeDelta);
	void Fix_Mouse();
	void AnimationState(_double dTimeDelta);
	void Motion_Change(ANIMATIONFLAG eAnimationFlag);
	void WallRunCameraReset(_double dTimeDelta);
	void Add_Collisions();
	void Attack();

	_bool Check_Hook(_double dTimeDelta);

private: /* Late_Tick */
	void CameraOffset(_double dTimeDelta);

private: /* Collisions */
	void CollisionStayWall(COLLISIONDESC CollisionDesc);

private: /* Skills */
	void Tick_Skills(_double dTimeDelta);
	void Dash(_double dTimeDelta);

private: /* Setup Files */
	HRESULT SetUp_AnimationNotifies(const _tchar* pNotifyFilePath);
	HRESULT SetUp_Collider(const _tchar* pColliderFilePath);

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

/*

62 - Hook Pull

84 - Att R1
85 - Att R2
86 - Att R3
87 - Att L1
88 - Att L2
89 - Att L3
92 - crouch idle
93 - crouch run
94 - crouch walk
95 - idle

96 - jump start
97 - jump loop
98 - jump end

103 - run wall L
104 - run wall R

231 - climb

233 - drone ride end
237 - drone ride loop
239 - drone ride start

*/