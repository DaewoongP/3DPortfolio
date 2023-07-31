#pragma once
#include "Part.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
END

BEGIN(Client)

class CWire final : public CPart
{
private:
	explicit CWire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CWire(const CWire& rhs);
	virtual ~CWire() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_ParentMatrix(PARENTMATRIXDESC ParentDesc) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Render_Wire(_fvector vHookPosition, _double dRenderTime);

private:
	CModel*				m_pModelCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };

private:
	_float4				m_vHookPos;
	_double				m_dRenderTimeAcc = { 0.0 };
	_double				m_dRenderTime = { 0.0 };

private:
	_float4				m_vEmissiveColor;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CWire* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END