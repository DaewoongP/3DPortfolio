#pragma once
#include "ImWindow.h"

BEGIN(Tool)

class CWindow_Model : public CImWindow
{
private:
	explicit CWindow_Model();
	virtual ~CWindow_Model() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;


private:
	HRESULT	Select_MeshFiles();
	HRESULT Setting_Transform();

public:
	static CWindow_Model* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END