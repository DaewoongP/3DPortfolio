// CameraTab.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "CameraTab.h"
#include "afxdialogex.h"


// CCameraTab 대화 상자

IMPLEMENT_DYNAMIC(CCameraTab, CDialogEx)

CCameraTab::CCameraTab(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TAB_CAMERA, pParent)
{

}

CCameraTab::~CCameraTab()
{
}

void CCameraTab::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCameraTab, CDialogEx)
END_MESSAGE_MAP()


// CCameraTab 메시지 처리기
