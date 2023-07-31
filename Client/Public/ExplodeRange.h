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

class CExplodeRange final : public CGameObject
{
private:
	explicit CExplodeRange(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CExplodeRange() = default;

public:
	virtual HRESULT Initialize(_fvector vPos, _float fRange, _double dExplodeTime);
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CVIBuffer_Rect*		m_pBufferCom = { nullptr };

private:
	_float			m_fRange = { 0.f };
	_double			m_dExplodeTimeAcc = { 0.0 };
	_double			m_dExplodeTime = { 0.0 };

public:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CExplodeRange* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _fvector vPos, _float fRange, _double dExplodeTime);
	virtual CGameObject* Clone(void* pArg) { return nullptr; }
	virtual void Free() override;
};

END