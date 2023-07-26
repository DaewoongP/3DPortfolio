#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CVIBuffer_Rect_Dynamic;
END

BEGIN(Client)

class CBoss_Hp final : public CUI
{
private:
	explicit CBoss_Hp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBoss_Hp(const CBoss_Hp& rhs);
	virtual ~CBoss_Hp() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iLevelIndex) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CBoss*		m_pBoss = { nullptr };

private:
	CVIBuffer_Rect_Dynamic*		m_pDynamicBufferCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CBoss_Hp* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END