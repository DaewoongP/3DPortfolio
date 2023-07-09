#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CBehaviorTree final : public CComponent
{
private:
	explicit CBehaviorTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBehaviorTree(const CBehaviorTree& rhs);
	virtual ~CBehaviorTree() = default;

public:
	const class CBehavior* Get_RootNode() const { return m_pRootNode; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;

public:
	virtual HRESULT Reset() override;

private:
	class CBehavior*		m_pRootNode = { nullptr };

public:
	static CBehaviorTree* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END