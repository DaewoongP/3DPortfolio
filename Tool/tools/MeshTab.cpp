// MeshTab.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "MeshTab.h"
#include "afxdialogex.h"


// CMeshTab 대화 상자

IMPLEMENT_DYNAMIC(CMeshTab, CDialogEx)

CMeshTab::CMeshTab(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TAB_MESH, pParent)
{

}

CMeshTab::~CMeshTab()
{
}

void CMeshTab::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_STATIC_MESH, m_StaticMeshTree);
	DDX_Control(pDX, IDC_TREE_DYNAMIC_MESH, m_DynamicMeshTree);
}


BEGIN_MESSAGE_MAP(CMeshTab, CDialogEx)
END_MESSAGE_MAP()


// CMeshTab 메시지 처리기
BOOL CMeshTab::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	if (FAILED(InitStaticMeshTree()))
		MessageBox(TEXT("Failed Create Static Mesh Tree"));
	if (FAILED(InitDynamicMeshTree()))
		MessageBox(TEXT("Failed Create Dynamic Mesh Tree"));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

HRESULT CMeshTab::InitStaticMeshTree()
{
	HTREEITEM hRoot, hChild1, hChild2;

	hRoot = m_StaticMeshTree.InsertItem(TEXT("Static_Mesh"), 0, 0, TVI_ROOT, TVI_LAST);
	hChild1 = m_StaticMeshTree.InsertItem(TEXT("child1"), 0, 0, hRoot, TVI_LAST);
	hChild2 = m_StaticMeshTree.InsertItem(TEXT("child2"), 0, 0, hRoot, TVI_LAST);

	m_StaticMeshTree.InsertItem(TEXT("last child1"), 0, 0, hChild1, TVI_LAST);
	m_StaticMeshTree.InsertItem(TEXT("last child2"), 0, 0, hChild2, TVI_LAST);

	return S_OK;
}

HRESULT CMeshTab::InitDynamicMeshTree()
{
	HTREEITEM hRoot;

	hRoot = m_DynamicMeshTree.InsertItem(TEXT("Dynamic_Mesh"), 0, 0, TVI_ROOT, TVI_LAST);
	return S_OK;
}
