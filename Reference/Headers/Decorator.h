#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CDecorator final : public CBase
{
private:
	explicit CDecorator() = default;
	virtual ~CDecorator() = default;

public:
	HRESULT Add_Decorator(function<_bool(class CBlackBoard*)> func);
	_bool	Check_Decorator(class CBlackBoard* pBlackboard);

private:
	list<function<_bool(class CBlackBoard*)>> m_Decorators;

public:
	static CDecorator* Create(function<_bool(class CBlackBoard*)> func);
	virtual void Free() override;
};

END