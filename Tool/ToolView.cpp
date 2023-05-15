
// ToolView.cpp: CToolView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "Tool.h"
#endif

#include "ToolDoc.h"
#include "ToolView.h"
#include "GameInstance.h"
#include "MainFrm.h"

// CToolView

IMPLEMENT_DYNCREATE(CToolView, CView)

BEGIN_MESSAGE_MAP(CToolView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CToolView 생성/소멸
// 글로벌 변수처리
HWND	g_hWnd;

CToolView::CToolView() noexcept
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

CToolView::~CToolView()
{
}

#pragma region Pass
BOOL CToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}
BOOL CToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}
void CToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}
void CToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}
#ifdef _DEBUG
void CToolView::AssertValid() const
{
	CView::AssertValid();
}
void CToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
CToolDoc* CToolView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CToolDoc)));
	return (CToolDoc*)m_pDocument;
}
#endif //_DEBUG
#pragma endregion

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// View Init	//////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
void CToolView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// AfxGetMainWnd : 현재의 메인 윈도우를 반환하는 전역함수
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();

	RECT	rcWnd{};
	pMainFrm->GetWindowRect(&rcWnd);
	SetRect(&rcWnd, 0, 0, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top);

	RECT rcMainView{};
	GetClientRect(&rcMainView);

	_float	fRowFrm = _float(rcWnd.right - rcMainView.right);
	_float	fColFrm = _float(rcWnd.bottom - rcMainView.bottom);

	pMainFrm->SetWindowPos(nullptr, 0, 0, _int(g_iWinSizeX + fRowFrm), _int(g_iWinSizeY + fColFrm), SWP_NOZORDER);

	g_hWnd = m_hWnd;
	//////////////////////// 디바이스 초기화 /////////////////////////
	GRAPHICDESC		GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof GraphicDesc);

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.iViewportSizeX = g_iWinSizeX;
	GraphicDesc.iViewportSizeY = g_iWinSizeY;
	GraphicDesc.eWinMode = GRAPHICDESC::WINMODE::WM_WIN;

	if (FAILED(m_pGameInstance->Initialize_Engine(static_cast<_uint>(LEVELID::LEVEL_END), GraphicDesc, &m_pDevice, &m_pContext)))
		AfxMessageBox(L"Failed Initialize_Engine");

}
// CToolView 그리기

void CToolView::OnDraw(CDC* /*pDC*/)
{
	CToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();
	m_pGameInstance->Present();
}


void CToolView::OnDestroy()
{
	CView::OnDestroy();

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	Safe_Release(m_pGameInstance);
	CGameInstance::Release_Engine();
}
