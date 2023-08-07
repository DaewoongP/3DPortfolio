#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CBloodScreen final : public CUI
{
private:
	explicit CBloodScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBloodScreen(const CBloodScreen& rhs);
	virtual ~CBloodScreen() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Render_Effect(_double dRenderTime);

private:
	_double						m_dRenderTimeAcc = { 0.0 };
	_double						m_dRenderTime = { 0.0 };
	_uint						m_iTextureIndex = { 0 };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CBloodScreen* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END