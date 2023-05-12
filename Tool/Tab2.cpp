// Tab2.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "Tab2.h"
#include "afxdialogex.h"


// CTab2 대화 상자

IMPLEMENT_DYNAMIC(CTab2, CDialogEx)

CTab2::CTab2(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1_TAB2, pParent)
{

}

CTab2::~CTab2()
{
}

void CTab2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTab2, CDialogEx)
END_MESSAGE_MAP()


// CTab2 메시지 처리기
