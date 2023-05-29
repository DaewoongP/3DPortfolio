#pragma once

#include "Tool_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Tool)
class CImWindow_Manager;

class CImWindow abstract : public CBase
{
protected:
    explicit CImWindow(ImGuiIO* pIO);
    virtual ~CImWindow() = default;

public:
    _bool   m_IsShow;

public:
    virtual void Initialize(void* pArg);
    virtual void Tick(_double dTimeDelta) PURE;

protected:
    ImGuiIO*                m_pIO = { nullptr };
    CGameInstance*          m_pGameInstance = { nullptr };
    CImWindow_Manager*      m_pImWindow_Manager = { nullptr };

public:
    virtual void Free(void) override;
};
END