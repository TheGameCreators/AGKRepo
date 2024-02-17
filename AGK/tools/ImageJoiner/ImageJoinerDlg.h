// ImageJoinerDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CImageJoinerDlg dialog
class CImageJoinerDlg : public CDialog
{
// Construction
public:
	CImageJoinerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_IMAGEJOINER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	void SetupD3D( );
	void CleanD3D( );

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_editImageFolder;
	afx_msg void OnBrowse();
	afx_msg void OnPack();
	CEdit m_editWidth;
	CEdit m_editHeight;
	CEdit m_editPadding;
};
