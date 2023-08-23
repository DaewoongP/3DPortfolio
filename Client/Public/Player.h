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
	enum STATE { 
		STATE_IDLE, STATE_ATTACK, STATE_RUN, 
		STATE_RUNWALL_L, STATE_RUNWALL_R, STATE_DASH, 
		STATE_CROUCH, STATE_HOOK, STATE_CLIMB,
		STATE_DRONRIDE, STATE_BLOCK, STATE_DEAD, 
		STATE_WEAPON, STATE_BLINK,
		STATE_END 
	};

	enum WEAPON {
		WEAPON_KATANA,
		WEAPON_SHURIKEN,
		WEAPON_END
	};
	
	enum SKILL {
		SKILL_BLINK,
		SKILL_SURGE,
		SKILL_END
	};

private:
	explicit CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	STATE Get_CurrentState() const { return m_eCurState; }
	SKILL Get_CurrentSkill() const { return m_eCurrentSkill; }
	const vector<const class CGameObject*>* Get_BlinkTarget() const { return &m_BlinkEnemys; }
	_float Get_SkillStackPercent() const { return (_float)m_iSkillStack / m_iSkillMaxStack; }
	_bool IsDashCoolTime() const { return m_Dash.isCoolTime; }
	void Gain_SkillStack() { 
		if (m_iSkillStack > m_iSkillMaxStack)
			m_iSkillStack = m_iSkillMaxStack;
		else
			++m_iSkillStack; 
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iLevelIndex) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual void OnCollisionEnter(COLLISIONDESC CollisionDesc) override;
	virtual void OnCollisionStay(COLLISIONDESC CollisionDesc) override;
	virtual void OnCollisionExit(COLLISIONDESC CollisionDesc) override;
	virtual HRESULT Render() override;
	virtual HRESULT Reset() override;

private:
	WEAPON					m_eCurWeapon = { WEAPON_END };
	_bool					m_bSwapWeapon = { false };

private:
	CModel*					m_pModelCom = { nullptr };
	CCamera*				m_pPlayerCameraCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	CShader*				m_pShadowShaderCom = { nullptr };
	CCollider*				m_pColliderCom = { nullptr };
	CRenderer*				m_pRendererCom = { nullptr };
	CNavigation*			m_pNavigationCom = { nullptr };

private:
	class CKatana*			m_pKatana = { nullptr };
	class CShuriken*		m_pShuriken = { nullptr };
	class CSurge*			m_pSurge = { nullptr };
	class CWire*			m_pWire = { nullptr };
	CCollider*				m_pVisionColliderCom = { nullptr };
	CCollider*				m_pBlockColliderCom = { nullptr };
	CCollider*				m_pBlinkColliderCom = { nullptr };

private:
	_float					m_fBlueScaleLevel = { 0.f };
	_float3					m_vInitRotation;
	_float4					m_vInitPosition;
	// 마우스 감도
	_float					m_fMouseSensitivity = { 0.f };
	// 현재 실행되고 있는 애니메이션 상태.
	ANIMATIONFLAG			m_eCurrentAnimationFlag = { ANIM_END };

	// 현재와 이전 상태를 비교해서 애니메이션 변경처리
	STATE					m_ePreState;
	STATE					m_eCurState;

	_bool					m_isCrouch = { false };

	_float					m_fSpeed = { 0.f };
	_float					m_fJumpPower = { 0.f };
	_float					m_fWallRunVelocity = { 0.f };
	_float					m_fHookPower = { 0.f };
	DASHDESC				m_Dash;

	_float					m_fAttackCamMove = { 0.f };

private: // Skill
	_uint					m_iSkillStack = { 0 };
	_uint					m_iSkillMaxStack = { 0 };
	SKILL					m_eCurrentSkill = { SKILL_END };
	_bool					m_isBlink = { false };
	_double					m_dBlinkTimeAcc = { 0.0 };
	_double					m_dBlinkTime = { 0.0 };

private: // WallRun
	_float					m_fWallRunY = { 0.f };
	_float					m_fWallRunAngle = { 0.f };
	_bool					m_isWallRotated = { false };
	_float3					m_vWallDir;
	_bool					m_isWallRun = { false };

private: // Block
	_bool					m_isBlocked = { false };

private:
	vector<const CCollider*>	m_InRangeEnemyColliders;
	vector<const CGameObject*>	m_BlockEnemyWeapons;
	vector<const CGameObject*>	m_BlinkEnemys;
	_float3					m_vAttackPositon;

private:
	_float4x4				m_CameraMatrix;

private:
	vector<class CGameObject*>	m_Shurikens;

private: /* Effects */
	class CBlockEffect*		m_pBlockEffect = { nullptr };

#ifdef _DEBUG
	_bool					m_isMouseFixed = { true };
	_bool					m_isInvisible = { false };
#endif // _DEBUG

private:
	HRESULT Add_Component();
	HRESULT Add_Component_Level(_uint iLevelIndex);
	HRESULT Add_Parts(_uint iLevelIndex);
	HRESULT SetUp_ShaderResources();
	HRESULT Initailize_Skills();

	void Key_Input(_double dTimeDelta);
	void Fix_Mouse();
	void AnimationState(_double dTimeDelta);
	void Motion_Change(ANIMATIONFLAG eAnimationFlag);
	void WallRunCameraReset(_double dTimeDelta);
	void Add_Collisions();
	void Attack(_double dTimeDelta);
	void Block(_double dTimeDelta);

	_bool Check_Hook(_double dTimeDelta);
	void CameraMove(_double dTimeDelta);
	void SwapWeapon();
	void Check_Fall();

	void SoundState(_double dTimeDelta);

private:
	void CameraOffset(_double dTimeDelta);

private: /* Collisions */
	void CollisionStayWall(COLLISIONDESC CollisionDesc);

private: /* Skills */
	void Tick_Skills(_double dTimeDelta);
	void Dash(_double dTimeDelta);
	void Blink(_double dTimeDelta);
	void Surge(_double dTimeDelta);

private: /* Setup Files */
	HRESULT Add_Notifies();
	HRESULT SetUp_AnimationNotifies(const _tchar* pNotifyFilePath);
	HRESULT SetUp_Collider(const _tchar* pColliderFilePath);

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

/*

118 - Blink aim

169~178 - Dead

62 - Hook Pull

80 - Block
81 - Block2
82 - Block3

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

/*

142 - SH IDLE
129 - SH ATTACK
130 - SH BLOCK
131 - SH CROUCH
132 - SH RUN
145 - SH RUN WALL L
146 - SH RUN WALL R

147 - KAT To SH
148 - SH To KAT

*/