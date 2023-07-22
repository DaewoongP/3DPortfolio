#pragma once
#include "Enemy.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CBehaviorTree;
END

BEGIN(Client)

class CEnemy_Sword final : public CEnemy
{
public:
	enum STATE { STATE_IDLE, STATE_READY, STATE_DASH, STATE_WALK, STATE_BLOCK, STATE_DEAD, STATE_END };

private:
	explicit CEnemy_Sword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEnemy_Sword(const CEnemy_Sword& rhs);
	virtual ~CEnemy_Sword() = default;

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
	CModel*				m_pModelCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CCollider*			m_pColliderCom = { nullptr };
	CCollider*			m_pVisionColliderCom = { nullptr };
	CBehaviorTree*		m_pBehaviorTreeCom = { nullptr };

private:
	class CSword*			m_pSword = { nullptr };
	_float					m_fVisionRange = { 0.f };	

private: /* BehaviorTree */
	const class CGameObject*	m_pTargetPlayer = { nullptr };
	_bool						m_isWalk = { false };
	_double						m_dAttackCoolTime = { 0.0 };
	_double						m_dPatrolWaitTime = { 0.0 };
	_bool						m_isWait = { false };
	_bool						m_isDash = { false };
	_double						m_dDashTime = { 0.0 };
	_double						m_dDashSpeed = { 0.0 };
	_bool						m_isReady = { false };
	_double						m_dReadyTime = { 0.0 };

private:
	// ���� ����ǰ� �ִ� �ִϸ��̼� ����.
	ANIMATIONFLAG			m_eCurrentAnimationFlag = { ANIM_END };

	// ����� ���� ���¸� ���ؼ� �ִϸ��̼� ����ó��
	STATE					m_ePreState = { STATE_END };
	STATE					m_eCurState = { STATE_IDLE };

	_bool					m_isBlocked = { false };

private: /* Tick */
	HRESULT Add_Component(ENEMYDESC& EnemyDesc);
	HRESULT Add_Parts(_uint iLevelIndex);
	HRESULT SetUp_BehaviorTree();
	HRESULT SetUp_ShaderResources();

	void AnimationState(_double dTimeDelta);
	void Motion_Change(ANIMATIONFLAG eAnimationFlag);

	void Attack();

private: /* Late_Tick */
	GAMEEVENT PlayEvent(_double dTimeDelta);

public:
	void Blocked();

public:
	static CEnemy_Sword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END


/*

1 - Idle

5 - walk Forward (���� ������)

13 - death1
14 - death2
15 - death3

19 - ready dash attack
20 - dash loop
21 - dash attack

23 - Blocked

*/