#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Line;
END

BEGIN(Tool)

class CAxis final : public CGameObject
{
private:
	explicit CAxis(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CAxis(const CAxis& rhs);
	virtual ~CAxis() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*		m_pShaderCom = { nullptr };
	CRenderer*		m_pRendererCom = { nullptr };
	CVIBuffer_Line* m_pLineCom = { nullptr };

private:
	HRESULT		Add_Components();
	HRESULT		SetUp_ShaderResources();

public:
	static CAxis* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END