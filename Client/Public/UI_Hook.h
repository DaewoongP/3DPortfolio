#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Point_Instance;
END

BEGIN(Client)

class CUI_Hook final : public CGameObject
{
private:
	explicit CUI_Hook(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Hook(const CUI_Hook& rhs);
	virtual ~CUI_Hook() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iLevelIndex) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

private: /* Hook Texture */
	CShader*						m_pShaderCom = { nullptr };
	CTexture*						m_pTextureCom = { nullptr };
	CRenderer*						m_pRendererCom = { nullptr };
	CVIBuffer_Point_Instance*		m_pBufferCom = { nullptr };

private:
	list<_float3>		m_HookPositions;
	_uint				m_iAllHookCnt = { 0 };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CUI_Hook* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(
	) override;
};

END