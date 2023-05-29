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
    explicit CImWindow();
    virtual ~CImWindow() = default;

public:
    _bool   m_IsShow;

public:
    virtual HRESULT Initialize(void* pArg) { return S_OK; };
    virtual void Tick(_double dTimeDelta) PURE;

protected:
    CGameInstance*          m_pGameInstance = { nullptr };
    CImWindow_Manager*      m_pImWindow_Manager = { nullptr };

public:
    virtual void Free(void) override;
};
END