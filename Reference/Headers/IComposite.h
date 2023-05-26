#pragma once
#include "Base.h"

interface IComposite : public CBase
{
protected:
	virtual ~IComposite() = default;

public:
	virtual void Tick(_double dTimeDelta) {}
	virtual void Late_Tick(_double dTimeDelta) {}
	virtual void Render() {}

public:
	virtual void Free() PURE;
};

