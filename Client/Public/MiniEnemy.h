#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Client)

class CMiniEnemy final : public CUI
{
private:
	explicit CMiniEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMiniEnemy(const CMiniEnemy& rhs);
	virtual ~CMiniEnemy() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iLevelIndex) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float					m_fMaxEnemyDistance = { 0.f };
	// 미니맵 센터 포지션
	_float2					m_vUIPos;
	// 실제 게임상 거리에 곱할 오프셋 변수
	_float2					m_vOffSet;
	list<CTransform*>		m_EnemyTransforms;
	list<_float2>			m_UIPositions;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CMiniEnemy* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END