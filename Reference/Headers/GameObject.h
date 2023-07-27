#pragma once
#include "Composite.h"
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CComposite
{
protected:
	explicit CGameObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	_bool			Is_Dead() const { return m_isDead; }
	class CTransform* Get_Transform() const { return m_pTransformCom; }
	const _tchar*	Get_Tag() const { return m_pTag; }
	const _tchar*	Get_LayerTag() const { return m_pLayerTag; }
	GAMEEVENT		Get_GameEvent() const { return m_eGameEvent; }
	void			Set_Dead() { m_isDead = true; }
	void			Set_Tag(const _tchar * pTag) { lstrcpy(m_pTag, pTag); }
	void			Set_LayerTag(const _tchar* pTag) { lstrcpy(m_pLayerTag, pTag); }
	void			Set_GameEvent(GAMEEVENT eEvent) { m_eGameEvent = eEvent; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg, CTransform::TRANSFORMDESC* pTransformDesc);
	virtual HRESULT Initialize_Level(_uint iLevelIndex);
	virtual void	Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual void OnCollisionEnter(COLLISIONDESC CollisionDesc) {}
	virtual void OnCollisionStay(COLLISIONDESC CollisionDesc) {}
	virtual void OnCollisionExit(COLLISIONDESC CollisionDesc) {}
	virtual HRESULT Render() override;
	virtual HRESULT Reset() override;

protected:
	_tchar		m_pTag[MAX_STR] = TEXT("");
	_tchar		m_pLayerTag[MAX_STR] = TEXT("");
	GAMEEVENT	m_eGameEvent = { GAME_END };
	_bool		m_isDead = { false };

protected:
	class CTransform*			m_pTransformCom = { nullptr };

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END