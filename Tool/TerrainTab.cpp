// TerrainTab.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "TerrainTab.h"
#include "afxdialogex.h"
#include "ToolInstance.h"

// CTerrainTab 대화 상자

IMPLEMENT_DYNAMIC(CTerrainTab, CDialogEx)

CTerrainTab::CTerrainTab(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TAB_TERRAIN, pParent)
	, m_pToolInstance{ CToolInstance::GetInstance() }
{
	Safe_AddRef(m_pToolInstance);
}

CTerrainTab::~CTerrainTab()
{
	Safe_Release(m_pToolInstance);
}

void CTerrainTab::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT_TERRAIN_SIZE_X, m_iTerrainSizeX);
	m_iTerrainSizeX.SetWindowTextW(_T("5"));

	DDX_Control(pDX, IDC_EDIT_TERRAIN_SIZE_Z, m_iTerrainSizeZ);
	m_iTerrainSizeZ.SetWindowTextW(_T("5"));

	DDX_Control(pDX, IDC_BUTTON_WIREFRAME, m_WireFrameBtn);
	m_WireFrameBtn.SetWindowTextW(TEXT("WireFrame"));

	DDX_Control(pDX, IDC_EDIT_CAM_SPEED, m_CameraMoveSpeed);
	m_CameraMoveSpeed.SetWindowTextW(_T("5.0"));
}

BEGIN_MESSAGE_MAP(CTerrainTab, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_TERRAIN_SIZE_APPLY, &CTerrainTab::OnBnClickedButtonTerrainSizeApply)
	ON_BN_CLICKED(IDC_BUTTON_WIREFRAME, &CTerrainTab::OnBnClickedButtonWireframe)
	ON_BN_CLICKED(IDC_BUTTON_CAMERA_SPEED, &CTerrainTab::OnBnClickedButtonCameraSpeed)
END_MESSAGE_MAP()


// CTerrainTab 메시지 처리기

void CTerrainTab::OnBnClickedButtonTerrainSizeApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strTerrainX, strTerrainZ;
	m_iTerrainSizeX.GetWindowTextW(strTerrainX);
	m_iTerrainSizeZ.GetWindowTextW(strTerrainZ);
	if (nullptr == m_pToolInstance ||
		nullptr == m_pToolInstance->m_pTerrain)
		return;
	m_pToolInstance->m_pTerrain->RemakeTerrain(_ttoi(strTerrainX), _ttoi(strTerrainZ));
}


void CTerrainTab::OnBnClickedButtonWireframe()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (nullptr == m_pToolInstance ||
		nullptr == m_pToolInstance->m_pTerrain)
		return;
	m_bIsWireFrame = !m_bIsWireFrame;
	if (m_bIsWireFrame)
		m_WireFrameBtn.SetWindowTextW(TEXT("WireFrame"));
	else
		m_WireFrameBtn.SetWindowTextW(TEXT("Solid"));

	m_pToolInstance->m_pTerrain->Set_WireFrame(m_bIsWireFrame);
}


void CTerrainTab::OnBnClickedButtonCameraSpeed()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (nullptr == m_pToolInstance ||
		nullptr == m_pToolInstance->m_pDynamicCamera)
		return;
	CString strCamSpeed;
	m_CameraMoveSpeed.GetWindowTextW(strCamSpeed);
	_float fSpeed = (_float)_ttof(strCamSpeed);
	m_pToolInstance->m_pDynamicCamera->Set_CamSpeed(fSpeed);
}
