#pragma once


// CMeshTab 대화 상자

class CMeshTab : public CDialogEx
{
	DECLARE_DYNAMIC(CMeshTab)

public:
	CMeshTab(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CMeshTab();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAB_MESH };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

private:
	CTreeCtrl m_StaticMeshTree;
	CTreeCtrl m_DynamicMeshTree;

private:
	HRESULT InitStaticMeshTree();
	HRESULT InitDynamicMeshTree();
};
