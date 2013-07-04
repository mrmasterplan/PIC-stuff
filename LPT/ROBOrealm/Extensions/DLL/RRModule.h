// RRModule.h : main header file for the RRMODULE DLL
//

#if !defined(AFX_RRMODULE_H__8CCBA742_E661_4253_B54C_1D6CB4285E1E__INCLUDED_)
#define AFX_RRMODULE_H__8CCBA742_E661_4253_B54C_1D6CB4285E1E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CRRModuleApp
// See RRModule.cpp for the implementation of this class
//

class CRRModuleApp : public CWinApp
{
public:
	CRRModuleApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRRModuleApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CRRModuleApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RRMODULE_H__8CCBA742_E661_4253_B54C_1D6CB4285E1E__INCLUDED_)
