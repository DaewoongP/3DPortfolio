#pragma once
#include "Tool_Defines.h"
#include "GameInstance.h"
#include "Base.h"

BEGIN(Tool)

class CImWindow abstract : public CBase
{
protected:
    explicit CImWindow();
    virtual ~CImWindow() = default;

public:
    virtual HRESULT Initialize(void* pArg = nullptr) { return S_OK; };
    virtual void Tick(_double dTimeDelta);

protected:
    CGameInstance*          m_pGameInstance = { nullptr };
	ImGuiWindowFlags		m_WindowFlag;
    ImVec2                  m_vWindowPos;
    ImVec2                  m_vWindowSize;

public:
    virtual void Free(void) override;
};
END