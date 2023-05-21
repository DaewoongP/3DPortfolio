#pragma once


// CTerrainTab 대화 상자
BEGIN(Tool)
class CToolInstance;
END

class CTerrainTab : public CDialogEx
{
	DECLARE_DYNAMIC(CTerrainTab)

public:
	CTerrainTab(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTerrainTab();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAB_TERRAIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonTerrainSizeApply();

private:
	CToolInstance*	m_pToolInstance = { nullptr };
	CEdit m_iTerrainSizeX;
	CEdit m_iTerrainSizeZ;
};
