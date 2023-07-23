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
	enum STATE { STATE_IDLE, STATE_DEAD, STATE_END };

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

public:
	void Throw_Bomb(_fvector vTargetPosition);

private:
	CModel*						m_pModelCom = { nullptr };
	CShader*					m_pShaderCom = { nullptr };
	CCollider*					m_pColliderCom = { nullptr };
	CCollider*					m_pVisionColliderCom = { nullptr };
	CBehaviorTree*				m_pBehaviorTreeCom = { nullptr };

private: /* Parts */
	class CBoss_Sword*			m_pSword = { nullptr };
	vector<CGameObject*>		m_Bombs;

private: /* BehaviorTree */
	const class CGameObject*	m_pTargetPlayer = { nullptr };
	/* Task Fly */
	_float						m_fFlyHeight = { 0.f };
	_float						m_fFlySpeed = { 0.f };
	/* RandomChoose_Move */
	_bool						m_isMoveFront = { false };
	_bool						m_isMoveBack = { false };
	_bool						m_isMoveRight = { false };
	_bool						m_isMoveLeft = { false };
	_double						m_dMoveTime = { 0.0 };

private:
	// 현재 실행되고 있는 애니메이션 상태.
	ANIMATIONFLAG				m_eCurrentAnimationFlag = { ANIM_END };

	// 현재와 이전 상태를 비교해서 애니메이션 변경처리
	STATE						m_ePreState = { STATE_END };
	STATE						m_eCurState = { STATE_IDLE };

private: /* Initialize */
	HRESULT Add_Component();
	HRESULT Add_Parts();
	HRESULT SetUp_BehaviorTree();
	HRESULT SetUp_ShaderResources();

private: /* Tick */
	void AnimationState(_double dTimeDelta);
	void Motion_Change(ANIMATIONFLAG eAnimationFlag);
	void Tick_Bomb(_double dTimeDelta);

private: /* Late_Tick */
	GAMEEVENT PlayEvent(_double dTimeDelta);
	void Late_Tick_Bomb(_double dTimeDelta);

public:
	static CBoss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END


/*

0 - charge_end
1 - charge_loop
2 - sequence end to fly
3 - fly back
4 - fly block
5 - fly front
6 - fly fall loop

7 - fly hit 1
8 - fly hit 2
9 - fly hit 3

10 - fly left
11 - fly loop
12 - fly right

13 - fly to idle

14 - idle loop

15 - attack sequence 1
16 - attack sequence 1 end

17 - attack sequence 2
18 - attack sequence 2 end

19 - idle to charge

20 - block loop (클릭 연타 부분)

21 - OTL loop

*/