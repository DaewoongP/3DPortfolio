#pragma once
#include "Enemy.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CBehaviorTree;
END

BEGIN(Client)

class CEnemy_Pistol final : public CEnemy
{
public:
	enum STATE { STATE_IDLE, STATE_ATTACK, STATE_RUN, STATE_RELOAD, STATE_DEAD, STATE_END };
private:
	explicit CEnemy_Pistol(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEnemy_Pistol(const CEnemy_Pistol& rhs);
	virtual ~CEnemy_Pistol() = default;

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
	CShader*				m_pShaderCom = { nullptr };
	CCollider*				m_pColliderCom = { nullptr };
	CCollider*				m_pVisionColliderCom = { nullptr };
	CBehaviorTree*			m_pBehaviorTreeCom = { nullptr };

private:
	class CPistol*			m_pPistol = { nullptr };
	_float					m_fVisionRange = { 0.f };
	_float					m_fBulletAcc = { 0.f };
	_float					m_fBulletTime = { 0.5f };
	_bool					m_isAttack = { false };

private:
	const class CGameObject*	m_pTargetPlayer = { nullptr };

private:
	// 현재 실행되고 있는 애니메이션 상태.
	ANIMATIONFLAG			m_eCurrentAnimationFlag = { ANIM_END };

	// 현재와 이전 상태를 비교해서 애니메이션 변경처리
	STATE					m_ePreState = { STATE_END };
	STATE					m_eCurState = { STATE_IDLE };

private: /* Tick */
	HRESULT Add_Component(ENEMYDESC& EnemyDesc);
	HRESULT Add_Parts();
	HRESULT SetUp_ShaderResources();

	void AnimationState(_double dTimeDelta);
	void Motion_Change(ANIMATIONFLAG eAnimationFlag);

private: /* Late_Tick */
	GAMEEVENT PlayEvent(_double dTimeDelta);

public:
	static CEnemy_Pistol* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

/*

거의 동일한거같은데 잘모르겠음. 둘중하나 사용
54 - Idle -> 일단이거로 통일함.
98 - Idle

55 - jog Backward
56 - jog Forward
57 - jog Left
58 - jog Right
59 - Reload

93 - turn 180 L
94 - turn 180 R
95 - turn 90 L
96 - turn 90 R

97 - Attack

51 - death1
52 - death2
53 - death3

*/