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

private:
	class CHammer*				m_pHammer = { nullptr };
	_float						m_fVisionRange = { 0.f };

private: /* BehaviorTree */
	const class CGameObject*	m_pTargetPlayer = { nullptr };

private:
	// ���� ����ǰ� �ִ� �ִϸ��̼� ����.
	ANIMATIONFLAG			m_eCurrentAnimationFlag = { ANIM_END };

	// ����� ���� ���¸� ���ؼ� �ִϸ��̼� ����ó��
	STATE					m_ePreState = { STATE_END };
	STATE					m_eCurState = { STATE_IDLE };

private: /* Tick */
	HRESULT Add_Component();
	HRESULT Add_Parts();
	HRESULT SetUp_BehaviorTree();
	HRESULT SetUp_ShaderResources();

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

53 - idle to swing
62 - swing(hit)
50 - hit to idle

68 - walk (���� ������)

*/