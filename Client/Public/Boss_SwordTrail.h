#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect_Trail;
END

BEGIN(Client)

class CBoss_SwordTrail final : public CGameObject
{
private:
	explicit CBoss_SwordTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBoss_SwordTrail(const CBoss_SwordTrail& rhs);
	virtual ~CBoss_SwordTrail() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Add_Render();

private:
	CShader*				m_pShaderCom = { nullptr };
	CTexture*				m_pTextureCom = { nullptr };
	CRenderer*				m_pRendererCom = { nullptr };
	CVIBuffer_Rect_Trail*	m_pTrailBufferCom = { nullptr };

private:
	HRESULT Add_Components(void* pArg);

public:
	static CBoss_SwordTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END