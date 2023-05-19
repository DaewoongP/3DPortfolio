#pragma once
#include "Base.h"

BEGIN(Engine9)

class CComponent_Manager final : public CBase
{
	DECLARE_SINGLETON(CComponent_Manager)
private:
	explicit CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(const _tchar* pPrototypeTag, void* pArg);

private:
	unordered_map<const _tchar*, class CComponent*>		m_Prototypes;
	
private:
	class CComponent* Find_Prototype(const _tchar* pPrototypeTag);

public:
	virtual void Free() override;
};

END