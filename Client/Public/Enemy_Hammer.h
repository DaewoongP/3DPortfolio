#pragma once
#include "Enemy.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CBehaviorTree;
END

BEGIN(Client)

class CEnemy_Hammer final : public CEnemy
{
public:
	enum STATE { STATE_IDLE, STATE_WALK, STATE_READY, STATE_ATTACK, STATE_WAIT, STATE_DEAD, STATE_END };

private:
	explicit CEnemy_Hammer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEnemy_Hammer(const CEnemy_Hammer& rhs);
	virtual ~CEnemy_Hammer() = default;

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
	virtual HRESULT Render_LightDepth() override;
	virtual HRESULT Reset() override;

private:
	CModel*						m_pModelCom = { nullptr };
	CShader*					m_pShaderCom = { nullptr };
	CShader*					m_pShadowShaderCom = { nullptr };
	CCollider*					m_pColliderCom = { nullptr };
	CCollider*					m_pVisionColliderCom = { nullptr };
	CBehaviorTree*				m_pBehaviorTreeCom = { nullptr };

private:
	class CHammer*				m_pHammer = { nullptr };
	_float						m_fVisionRange = { 0.f };

private: /* BehaviorTree */
	const class CGameObject*	m_pTargetPlayer = { nullptr };
	_bool						m_isWalk = { false };
	_bool						m_isWait = { false };
	_double						m_dMaxWaitTime = { 0.0 };
	_bool						m_isReady = { false };
	_double						m_dReadyTime = { 0.0 };
	_double						m_dAttackCoolTime = { 0.0 };
	_bool						m_isAttack = { false };
	_double						m_dWaitTime = { 0.0 };
	_float4x4					m_LensOffsetMatrix;

private:
	// 현재 실행되고 있는 애니메이션 상태.
	ANIMATIONFLAG			m_eCurrentAnimationFlag = { ANIM_END };

	// 현재와 이전 상태를 비교해서 애니메이션 변경처리
	STATE					m_ePreState = { STATE_END };
	STATE					m_eCurState = { STATE_IDLE };

private: /* Tick */
	HRESULT Add_Component();
	HRESULT Add_Parts(_uint iLevelIndex);
	HRESULT SetUp_BehaviorTree();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources();

	void	AnimationState(_double dTimeDelta);
	void	Motion_Change(ANIMATIONFLAG eAnimationFlag);

	void	Attack();

private: /* Late_Tick */
	GAMEEVENT PlayEvent(_double dTimeDelta);

public:
	static CEnemy_Hammer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

/*

52 - idle

44~49 - Death

53 - idle to swing (Ready)
62 - swing(Attack)
50 - hit to idle (wait)

68 - walk (로컬 움직임)

*/