#pragma once
#include "Enemy.h"

BEGIN(Engine)
class CModel;
class CShader;
END

BEGIN(Client)

class CEnemy_Sniper final : public CEnemy
{
public:
	enum STATE { STATE_IDLE, STATE_ATTACK, STATE_DEAD, STATE_END };

private:
	explicit CEnemy_Sniper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEnemy_Sniper(const CEnemy_Sniper& rhs);
	virtual ~CEnemy_Sniper() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iLevelIndex) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;
	virtual HRESULT Reset() override;

public:
	void Attack(_double dArriveTime);
	void Delete_Bullet();

private:
	CModel*						m_pModelCom = { nullptr };
	CShader*					m_pShaderCom = { nullptr };
	CShader*					m_pShadowShaderCom = { nullptr };
	class CBloodDirectional*	m_pBloodEffect = { nullptr };
	class CBloodParticle*		m_pBloodParticle = { nullptr };

private:
	class CSniper*				m_pSniper = { nullptr };

private:
	class CMiniPlayer*			m_pTargetPlayer = { nullptr };

private:
	// 현재 실행되고 있는 애니메이션 상태.
	ANIMATIONFLAG				m_eCurrentAnimationFlag = { ANIM_END };

	// 현재와 이전 상태를 비교해서 애니메이션 변경처리
	STATE						m_ePreState = { STATE_END };
	STATE						m_eCurState = { STATE_IDLE };

private: /* Tick */
	HRESULT Add_Component(ENEMYDESC & EnemyDesc);
	HRESULT Add_Parts(_uint iLevelIndex);
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources();

	void AnimationState(_double dTimeDelta);
	void Motion_Change(ANIMATIONFLAG eAnimationFlag);

private: /* Late_Tick */
	GAMEEVENT PlayEvent(_double dTimeDelta);

public:
	static CEnemy_Sniper* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END