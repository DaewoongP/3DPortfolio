#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CCamera;
class CShader;
class CRenderer;
END

BEGIN(Client)

class CMiniPlayer final : public CGameObject
{
public:
	enum STATE { STATE_IDLE, STATE_BLOCK, STATE_DEAD, STATE_END };

private:
	explicit CMiniPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMiniPlayer(const CMiniPlayer& rhs);
	virtual ~CMiniPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iLevelIndex) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Reset() override;

private:
	CModel*					m_pModelCom = { nullptr };
	CCamera*				m_pCameraCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	CRenderer*				m_pRendererCom = { nullptr };
	class CKatana*			m_pKatana = { nullptr };
	class CBlockEffect*		m_pBlockEffect = { nullptr };
	class CHitScreen*		m_pHitScreenEffect = { nullptr };

private:
	_float					m_fMouseSensitivity = { 0.f };
	ANIMATIONFLAG			m_eCurrentAnimationFlag = { ANIM_END };

	STATE					m_ePreState;
	STATE					m_eCurState;

#ifdef _DEBUG
	_bool					m_isMouseFixed = { false };
#endif // _DEBUG


private:
	HRESULT Add_Component();
	HRESULT Add_Parts(_uint iLevelIndex);
	HRESULT SetUp_ShaderResources();
	void Key_Input(_double dTimeDelta);
	void Fix_Mouse();
	void AnimationState(_double dTimeDelta);
	void Motion_Change(ANIMATIONFLAG eAnimationFlag);
	void CameraOffset(_double dTimeDelta);

public:
	static CMiniPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END