#pragma once

#include "Tool_Defines.h"
#include "Engine_Defines.h"
#include "Base.h"
#include "Window_Tool.h"
#include "Window_Camera.h"
#include "Window_UI.h"
#include "Window_Light.h"
#include "Window_Model.h"

BEGIN(Tool)

class CImWindow_Manager final : public CBase
{
    DECLARE_SINGLETON(CImWindow_Manager);
public:
    enum TOOLWINDOW 
    {
        TOOL,
        CAMERA,
        UI,
        LIGHT,
        MODEL,
        WINDOW_END
    };

private:
    explicit CImWindow_Manager();
    ~CImWindow_Manager() = default;

public:
    HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImGuiIO** pIO);
    void Tick(_double dTimeDelta);
    void Render();

private:
    ImGuiIO*                m_pIO = { nullptr };

public:
    vector<CImWindow*>      m_ImWindows;

public:
    virtual void Free(void) override;
};

END