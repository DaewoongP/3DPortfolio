#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect_Instance;
END

BEGIN(Client)

class CBlue_Snow final : public CGameObject
{
private:
	explicit CBlue_Snow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBlue_Snow(const CBlue_Snow& rhs);
	virtual ~CBlue_Snow() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*					m_pShaderCom = { nullptr };
	CTexture*					m_pTextureCom = { nullptr };
	CRenderer*					m_pRendererCom = { nullptr };
	CVIBuffer_Rect_Instance*	m_pVIBufferCom = { nullptr };

public:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CBlue_Snow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END