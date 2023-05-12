
// MainFrm.cpp: CMainFrame 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#include "Tool.h"

#include "MainFrm.h"
#include "GameInstance.h"
#include "ToolView.h"
#include "MyForm.h"

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame() noexcept
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	//CreateStatic : 분할 창을 만들기 위한 함수
	// CreateStatic(부모 윈도우 주소, 행의 수, 열의 수, 창 스타일, 자식창 id);

	m_MainSplitter.CreateStatic(this, 1, 2);

	// CreateView: 분할 창에 표시할 view 창을 만드는 함수
	//CreateView(배치할 행, 배치할 열, 배치할 view창을 새로 생성, 초기 크기, pContext)

	m_MainSplitter.CreateView(0, 0, RUNTIME_CLASS(CToolView), CSize(g_iWinSizeX - 250, g_iWinSizeY), pContext);
	m_MainSplitter.CreateView(0, 1, RUNTIME_CLASS(CMyForm), CSize(250, g_iWinSizeY), pContext);
	
	return TRUE;
}
