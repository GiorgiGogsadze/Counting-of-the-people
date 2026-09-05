
// Final ProjectDlg.h : header file
//

#pragma once


// CFinalProjectDlg dialog
class CFinalProjectDlg : public CDialogEx
{
// Construction
public:
	CFinalProjectDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FINAL_PROJECT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString file_link;
	afx_msg void OnBnClickedSelectvideo();
	afx_msg void OnBnClickedOk();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedSkip();
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnBnClickedSelectareaimage();
	afx_msg void OnBnClickedSelectdoorimage();
	CString area_link;
	CString doors_link;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedAreaselector();
	afx_msg void OnBnClickedDoorsselector();
};
