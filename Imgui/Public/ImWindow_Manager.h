#pragma once

#include "Tool_Defines.h"
#include "Engine_Defines.h"
#include "Base.h"
#include "ImWindow.h"

BEGIN(Tool)

class CImWindow_Manager final : public CBase
{
    DECLARE_SINGLETON(CImWindow_Manager);

private:
    explicit CImWindow_Manager();
    ~CImWindow_Manager() = default;

public:
    CImWindow* Get_ImWindow(wstring tag);

public:
    HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, __inout ImGuiIO** pIO);
    void Tick(_double dTimeDelta);
    void Render();
    HRESULT Add_Window(wstring tag, CImWindow* pWindow);
    CImWindow* Find_Window(wstring tag);

private:
    ImGuiIO* m_pIO = { nullptr };
    unordered_map<const _tchar*, CImWindow*>    m_ImWindows;

public:
    virtual void Free(void) override;
};

END