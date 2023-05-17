#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine9)
class CRenderer;
class CVIBuffer_RcCol;
END

BEGIN(Tool)

class CTerrain final : public CGameObject
{
private:
	explicit CTerrain(LPDIRECT3DDEVICE9 pDevice);
	explicit CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer*				m_pRendererCom = { nullptr };
	CVIBuffer_RcCol*		m_pRcColCom = { nullptr };

public:
	HRESULT		Add_Components();

public:
	static CTerrain* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END