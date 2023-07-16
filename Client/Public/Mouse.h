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

class CMouse final : public CGameObject
{
private:
	explicit CMouse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMouse(const CMouse& rhs);
	virtual ~CMouse() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	// ������â�� ���� x��ǥ
	_float			m_fX = { 0.f };
	// ������â�� ���� y��ǥ
	_float			m_fY = { 0.f };
	// �ؽ�ó x������ ����
	_float			m_fSizeX = { 0.f };
	// �ؽ�ó y������ ����
	_float			m_fSizeY = { 0.f };

	_float4x4		m_ViewMatrix;
	_float4x4		m_ProjMatrix;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CMouse* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END