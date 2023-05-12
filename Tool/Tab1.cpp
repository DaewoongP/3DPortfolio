// Tab1.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "Tab1.h"
#include "afxdialogex.h"

// CTab1 대화 상자

IMPLEMENT_DYNAMIC(CTab1, CDialogEx)

CTab1::CTab1(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1_TAB1, pParent)
{
}

CTab1::~CTab1()
{
}

void CTab1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTab1, CDialogEx)
END_MESSAGE_MAP()


// CTab1 메시지 처리기
