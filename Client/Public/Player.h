#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CCamera;
class CShader;
class CRenderer;
class CTransform;

END

BEGIN(Client)

class CPlayer final : public CGameObject
{
public:
	enum STATE { STATE_IDLE, STATE_END };
private:
	explicit CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual void Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CModel*					m_pModelCom = { nullptr };
	CCamera*				m_pPlayerFirstPersonViewCameraCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	CRenderer*				m_pRendererCom = { nullptr };
	CTransform*				m_pTransformCom = { nullptr };


private:
	_uint					m_iHeadChannelIndex = { 0 };

private:
	HRESULT Add_Component();
	HRESULT SetUp_ShaderResources();
	HRESULT Find_BoneIndices();

	void FirstPersonView();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END