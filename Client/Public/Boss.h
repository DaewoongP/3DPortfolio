#pragma once
#include "Enemy.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CBehaviorTree;
END

BEGIN(Client)

class CBoss final : public CEnemy
{
public:
	enum STATE { STATE_IDLE, STATE_END };

private:
	explicit CBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBoss(const CBoss& rhs);
	virtual ~CBoss() = default;

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
	CModel*						m_pModelCom = { nullptr };
	CShader*					m_pShaderCom = { nullptr };
	CCollider*					m_pColliderCom = { nullptr };
	CCollider*					m_pVisionColliderCom = { nullptr };
	CBehaviorTree*				m_pBehaviorTreeCom = { nullptr };

private: /* BehaviorTree */
	const class CGameObject*	m_pTargetPlayer = { nullptr };

private:
	// 현재 실행되고 있는 애니메이션 상태.
	ANIMATIONFLAG				m_eCurrentAnimationFlag = { ANIM_END };

	// 현재와 이전 상태를 비교해서 애니메이션 변경처리
	STATE						m_ePreState = { STATE_END };
	STATE						m_eCurState = { STATE_IDLE };

private: /* Initialize */
	HRESULT Add_Component(ENEMYDESC& EnemyDesc);
	HRESULT Add_Parts();
	HRESULT SetUp_BehaviorTree();
	HRESULT SetUp_ShaderResources();

private: /* Tick */
	void AnimationState(_double dTimeDelta);
	void Motion_Change(ANIMATIONFLAG eAnimationFlag);

private: /* Late_Tick */
	GAMEEVENT PlayEvent(_double dTimeDelta);

public:
	static CBoss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END