// MyForm.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "MyForm.h"

IMPLEMENT_DYNCREATE(CMyForm, CFormView)

CMyForm::CMyForm()
	: CFormView(IDD_CMyForm)
	, m_pTab1{ nullptr }
	, m_pTab2{ nullptr }
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
	m_Tab.InsertItem(0, L"test1");
	m_Tab.InsertItem(1, L"test2");
	// 탭 할당
	m_pTab1 = new CTab1;
	m_pTab1->Create(IDD_DIALOG1_TAB1, &m_Tab);
	m_pTab1->MoveWindow(0, 20, rc.Width(), rc.Height());
	m_pTab1->ShowWindow(true);

	m_pTab2 = new CTab2;
	m_pTab2->Create(IDD_DIALOG1_TAB2, &m_Tab);
	m_pTab2->MoveWindow(0, 20, rc.Width(), rc.Height());
	m_pTab2->ShowWindow(false);
}

void CMyForm::OnSelchangeTabMain(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (IDC_TAB_MAIN == pNMHDR->idFrom)
	{
		_int isel = m_Tab.GetCurSel();
		switch (isel)
		{
		case 0:
			m_pTab1->ShowWindow(true);
			m_pTab2->ShowWindow(false);
			break;
		case 1:
			m_pTab1->ShowWindow(false);
			m_pTab2->ShowWindow(true);
			break;
		}
	}
	*pResult = 0;
}



void CMyForm::OnDestroy()
{
	CFormView::OnDestroy();

	Safe_Delete(m_pTab1);
	Safe_Delete(m_pTab2);
}
