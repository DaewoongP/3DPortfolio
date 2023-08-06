#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CHOS final : public CGameObject
{
private:
	explicit CHOS(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CHOS(const CHOS& rhs);
	virtual ~CHOS() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*					m_pShaderCom = { nullptr };
	CTexture*					m_pTextureCom = { nullptr };
	CRenderer*					m_pRendererCom = { nullptr };
	CVIBuffer_Rect*				m_pBufferCom = { nullptr };

private:
	_float						m_fRotation = { 0.f };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CHOS* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END