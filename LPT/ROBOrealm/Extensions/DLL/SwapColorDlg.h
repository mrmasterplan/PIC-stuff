#if !defined(AFX_SWAPCOLORDLG_H__04140C98_2B0C_42C9_8AEE_1A79A6AAFFF4__INCLUDED_)
#define AFX_SWAPCOLORDLG_H__04140C98_2B0C_42C9_8AEE_1A79A6AAFFF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SwapColorDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SwapColorDlg dialog

class SwapColorDlg : public ModuleDlg
{
// Construction
public:
	SwapColorDlg(CWnd* pParent = NULL);   // standard constructor
	static int GetId();
	char *GetDisplay();
	int Process(unsigned char *data, short int *tmp, unsigned char *tmp2, int left, int top, int width, int height);
	void Annotate(unsigned char *data, short int *tmp2Image, unsigned char *tmpImage, int left, int top, int width, int height);
  int GetIDD(); 
  bool Initialize();
  void SaveValues();
  void LoadValues();

// Dialog Data
	//{{AFX_DATA(SwapColorDlg)
	enum { IDD = IDD_SWAPCOLOR };
	CString	m_myText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SwapColorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SwapColorDlg)
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SWAPCOLORDLG_H__04140C98_2B0C_42C9_8AEE_1A79A6AAFFF4__INCLUDED_)
