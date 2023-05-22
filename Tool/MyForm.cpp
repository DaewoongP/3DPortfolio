﻿// MyForm.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "MyForm.h"

IMPLEMENT_DYNCREATE(CMyForm, CFormView)

CMyForm::CMyForm()
	: CFormView(IDD_CMyForm)
{

}

CMyForm::~CMyForm()
{
}

void CMyForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_MAIN, m_Tab);
}

BEGIN_MESSAGE_MAP(CMyForm, CFormView)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MAIN, &CMyForm::OnSelchangeTabMain)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CMyForm 진단

#ifdef _DEBUG
void CMyForm::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CMyForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMyForm 메시지 처리기


void CMyForm::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	CRect rc;
	m_Tab.GetWindowRect(rc);
	m_Tab.SetCurSel(0);

	// 탭이름
	m_Tab.InsertItem(0, L"Terrain");
	m_Tab.InsertItem(1, L"Mesh");
	m_Tab.InsertItem(2, L"Camera");

	// 탭 할당
	m_pTerrainTab = new CTerrainTab;
	m_pTerrainTab->Create(IDD_TAB_TERRAIN, &m_Tab);
	m_vecTab.push_back(m_pTerrainTab);

	m_pMeshTab = new CMeshTab;
	m_pMeshTab->Create(IDD_TAB_MESH, &m_Tab);
	m_vecTab.push_back(m_pMeshTab);

	m_pCameraTab = new CCameraTab;
	m_pCameraTab->Create(IDD_TAB_CAMERA, &m_Tab);
	m_vecTab.push_back(m_pCameraTab);

	for (auto& pTab : m_vecTab)
		pTab->MoveWindow(0, 20, rc.Width(), rc.Height());

	m_vecTab[0]->ShowWindow(true);
}

void CMyForm::OnSelchangeTabMain(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (IDC_TAB_MAIN == pNMHDR->idFrom)
	{
		_uint iSel = m_Tab.GetCurSel();
		for (auto& pTab : m_vecTab)
		{
			pTab->ShowWindow(false);
		}
		m_vecTab[iSel]->ShowWindow(true);

	}
	*pResult = 0;
}



void CMyForm::OnDestroy()
{
	CFormView::OnDestroy();

	for (auto& pTab : m_vecTab)
		Safe_Delete(pTab);
	m_vecTab.clear();
}
