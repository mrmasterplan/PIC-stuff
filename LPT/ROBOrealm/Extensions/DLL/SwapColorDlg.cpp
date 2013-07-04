// SwapColorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include <vfw.h>
#include "HashtableExt.h"
#include "ModuleDlg.h"
#include "SwapColorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SwapColorDlg dialog


SwapColorDlg::SwapColorDlg(CWnd* pParent /*=NULL*/)
	: ModuleDlg(SwapColorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SwapColorDlg)
	m_myText = _T("");
	//}}AFX_DATA_INIT
	strcpy(name, "testsss");
}

void SwapColorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SwapColorDlg)
	DDX_Text(pDX, IDC_MYTEXT, m_myText);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SwapColorDlg, CDialog)
	//{{AFX_MSG_MAP(SwapColorDlg)
	ON_BN_CLICKED(IDC_SHOW_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SwapColorDlg message handlers

// Called on initialization of the dll
bool SwapColorDlg::Initialize()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

  // tell RoboRealm that we have a GUI dialog
  hasDialog = true;
	
	return true;
}

// when the ok button is pressed this is called and used to indicate in the RoboRealm program pipeline
// what this module is. You can use this routine to include basic indicators that help the end user
// know what operation is being performed.
char *SwapColorDlg::GetDisplay()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

  return "SwapColor";
}

// The main image processing call.
// data - contains the current image data as BGR pixels
// tmp2Image - a temporary short int buffer than can be used for storage during image processing
// tmpImage - a temporary char buffer than can be used for storage during image processing
int SwapColorDlg::Process(unsigned char *data, short int *tmp2Image, unsigned char *tmpImage, int left, int top, int width, int height)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

  int i;
  int c;

  // remember ... format is BGR (Blue, Green, Red) pixels
  for (i=0;i<(width*height*3);i+=3)
  {
	  c = data[i];
    data[i] = data[i+2];
    data[i+2] = c;
  }

  SetVariable("test", &m_myText);

  return 1;
}

// The annotation call. This is called after all processing routines are done to annotate the image .. i.e. draw lines, circles, etc.
// data - contains the current image data as BGR pixels
// tmp2Image - a temporary short int buffer than can be used for storage during image processing
// tmpImage - a temporary char buffer than can be used for storage during image processing
void SwapColorDlg::Annotate(unsigned char *data, short int *tmp2Image, unsigned char *tmpImage, int left, int top, int width, int height)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
}

// returns the IDD for this module. This is needed so that RoboRealm can correctly display the
// GUI dialog.
int SwapColorDlg::GetIDD()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

  return IDD_SWAPCOLOR;
}

void SwapColorDlg::SaveValues()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

  UpdateData(TRUE);
  SetString("my_text", &m_myText);
}

void SwapColorDlg::LoadValues()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	GetString("my_text", &m_myText);
  UpdateData(FALSE);
}


void SwapColorDlg::OnHelp()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

  ModuleDlg::OnHelp("http://www.roborealm.com/help/DLL_Program.php");
}
