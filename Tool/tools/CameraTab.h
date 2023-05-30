#pragma once


// CCameraTab 대화 상자

class CCameraTab : public CDialogEx
{
	DECLARE_DYNAMIC(CCameraTab)

public:
	CCameraTab(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CCameraTab();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAB_CAMERA };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
