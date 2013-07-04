#if !defined(AFX_ModuleDlg_H__C5647123_DDBE_404B_BD0B_95441968697E__INCLUDED_)
#define AFX_ModuleDlg_H__C5647123_DDBE_404B_BD0B_95441968697E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModuleDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ModuleDlg dialog

#define IDD_DEFAULT 0
#define P_EXTENSION 10000

class ModuleDlg : public CDialog
{
// Construction
public:
	ModuleDlg(int id, CWnd* pParent = NULL);   // standard constructor

  void SetInt(char *name, int value);
	int GetInt(char *name);
  void SetFloat(char *name, float value);
	float GetFloat(char *name);
	void GetString(char *name, CString *res);
	void SetString(char *name, CString *buffer);
	void SetString(char *name, char *buffer);
	void GetString(char *name, char *res);
	void SetVariable(char *name, int i);
	void SetVariableTable(HashtableExt *g);
  char *GetVariable(char *name);
  void SetVariable(char *name, char *val);
  void SetVariable(char *name, CString *val);
	void SetVariable(char *name, float f);
  void RemoveVariable(char *name);
	int SetVariableSelect(CComboBox *list, char *tmp);
	void AddVariable(CComboBox *list, char *s);
	void SetString(char *name, CComboBox *list, char *value=NULL);
	void SetArray(char *name, int *vals, int num);
	int GetArray(char *name, int *vals, int max);
  void SetDialog(void *dlg);
  void SetNode(HashtableExt *node);
	int skipSubComponent();
	void SetVariable(char *name, int *arr);

	virtual void SaveValues();
  virtual void LoadValues();
	virtual int Process(unsigned char *data, short int *tmp, unsigned char *tmp2, int left, int top, int width, int height);
	virtual void Annotate(unsigned char *data, short int *tmp, unsigned char *tmp2, int left, int top, int width, int height);
	virtual char *GetDisplay();
	virtual int GetProgramId();
	virtual int GetId();
	virtual char *GetName();

  virtual void OnOK();
  virtual void OnCancel();

	virtual void SetSize(int w, int h);
	virtual void SetDimension(int x, int y, int w, int h);
	virtual void UpdateImage();

	virtual void UpdateData(bool mode);

	virtual bool Initialize();
  virtual void Destroy();
  virtual int GetIDD();
	virtual BOOL ShowWindow(int cmd);

  virtual void RefreshValues();

  virtual enum { IDD = IDD_DEFAULT };

	BSTR AsciiToBSTR(const char* pszFName);
  void OnHelp(char *s);

	bool hasDialog;
	HashtableExt *global;
	char name[64];
	bool isActive;
	bool isInsert;

// Dialog Data
	//{{AFX_DATA(ModuleDlg)
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ModuleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	char display[256];
	int width, height, left, top;

	void *mainDlg;
	HashtableExt *myNode;

	// Generated message map functions
	//{{AFX_MSG(ModuleDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
  bool GetBool(char *name);
  void SetBool(char *name, bool value);
  void SetBool(char *name, BOOL value);
  int getImageNumber();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ModuleDlg_H__C5647123_DDBE_404B_BD0B_95441968697E__INCLUDED_)
