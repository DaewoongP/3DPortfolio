#pragma once

#include "Tool_Defines.h"
#include "Engine_Defines.h"
#include "Base.h"
#include "Window_Tool.h"
#include "Window_Camera.h"
#include "Window_UI.h"
#include "Window_Light.h"
#include "Window_Model.h"
#include "Window_Object.h"
#include "Window_Animation.h"

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
        OBJECT,
        ANIMATION,
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

#define TOOLWINDOW  \
dynamic_cast<CWindow_Tool*>(CImWindow_Manager::GetInstance()->m_ImWindows[CImWindow_Manager::TOOL])

#define CAMERAWINDOW  \
dynamic_cast<CWindow_Camera*>(CImWindow_Manager::GetInstance()->m_ImWindows[CImWindow_Manager::CAMERA])

#define UIWINDOW  \
dynamic_cast<CWindow_UI*>(CImWindow_Manager::GetInstance()->m_ImWindows[CImWindow_Manager::UI])

#define LIGHTWINDOW  \
dynamic_cast<CWindow_Light*>(CImWindow_Manager::GetInstance()->m_ImWindows[CImWindow_Manager::LIGHT])

#define MODELWINDOW  \
dynamic_cast<CWindow_Model*>(CImWindow_Manager::GetInstance()->m_ImWindows[CImWindow_Manager::MODEL])

#define OBJECTWINDOW  \
dynamic_cast<CWindow_Object*>(CImWindow_Manager::GetInstance()->m_ImWindows[CImWindow_Manager::OBJECT])

#define ANIMATIONWINDOW  \
dynamic_cast<CWindow_Animation*>(CImWindow_Manager::GetInstance()->m_ImWindows[CImWindow_Manager::ANIMATION])
