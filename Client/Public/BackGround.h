#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CComposite;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CBackGround final : public CGameObject
{
private:
	explicit CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBackGround(const CBackGround& rhs);
	virtual ~CBackGround() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual void Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*		m_pShaderCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };
	CRenderer*		m_pRendererCom = { nullptr };
	CTransform*		m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	// 윈도우창의 실제 x좌표
	_float			m_fX = { 0.f };
	// 윈도우창의 실제 y좌표
	_float			m_fY = { 0.f };
	// 텍스처 x사이즈 설정
	_float			m_fSizeX = { 0.f };
	// 텍스처 y사이즈 설정
	_float			m_fSizeY = { 0.f };

	_float4x4		m_ViewMatrix;
	_float4x4		m_ProjMatrix;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CBackGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END