// ModuleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
//#include "MyListBox.h"
//#include "OpenWith.h"
#include "HashtableExt.h"
#include "RoboRealmExt.h"
#include "ModuleDlg.h"

#define ARRAY_CONST_VALUE 13

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ModuleDlg dialog

ModuleDlg::ModuleDlg(int id, CWnd* pParent /*=NULL*/)
	: CDialog(id, pParent)
{
	//{{AFX_DATA_INIT(ModuleDlg)
	//}}AFX_DATA_INIT
  hasDialog=true;
  mainDlg=NULL;
	left=0;
	top=0;
  myNode=NULL;
  name[0]=0;
  display[0]=0;
	isActive=true;
	isInsert=true;
	width=320;
	height=240;
}

void ModuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ModuleDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(ModuleDlg, CDialog)
	//{{AFX_MSG_MAP(ModuleDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ModuleDlg message handlers

int ModuleDlg::GetProgramId()
{
  return 0;
}

int ModuleDlg::GetId()
{
  return 0;
}

char *ModuleDlg::GetDisplay()
{
  _snprintf(display, 256, "Blank");
  return display;
}

char *ModuleDlg::GetName()
{
  return "Blank";
}

int ModuleDlg::GetInt(char *name)
{
	if (myNode==NULL) return 0;
  char *txt = (char *)myNode->get(name);
  if (txt==NULL)
  	return 0;
  else
	  return (int)atoi(txt);
}

void ModuleDlg::SetInt(char *name, int value)
{
  char buffer[16];
	if (myNode==NULL) return;
	if (value==0)
	  myNode->remove(name);
	else
	{
		sprintf(buffer, "%d", value);
		myNode->put(name, buffer);
	}
}

float ModuleDlg::GetFloat(char *name)
{
	if (myNode==NULL) return 0;
  char *txt = (char *)myNode->get(name);
  if (txt==NULL)
	  return 0;
  else
	  return (int)atof(txt);
}

void ModuleDlg::SetFloat(char *name, float value)
{
  char buffer[16];
	if (myNode==NULL) return;
  sprintf(buffer, "%5.2f", value);
	if (stricmp(buffer, "0.00")==0)
	  myNode->remove(name);
	else
	  myNode->put(name, buffer);
}

void ModuleDlg::GetString(char *name, char *res)
{
	if (myNode==NULL) return;
  char *txt = (char *)myNode->get(name);
  if (txt!=NULL) strcpy(res, txt); else res[0]=0;
}

void ModuleDlg::GetString(char *name, CString *res)
{
	if (myNode==NULL) return;
  char *txt = (char *)myNode->get(name);
  if (txt!=NULL)
		*res = txt;
	else
		*res = "";
}

void ModuleDlg::SetString(char *name, CString *buffer)
{
	if (myNode==NULL) return;
	if (buffer->IsEmpty())
	  myNode->remove(name);
	else
	  myNode->put(name, (char *)((LPCTSTR)*buffer));
}

void ModuleDlg::SetString(char *name, char *buffer)
{
	if (myNode==NULL) return;
	if (buffer[0]==0)
		myNode->remove(name);
	else
		myNode->put(name, buffer);
}

void ModuleDlg::SetString(char *name, CComboBox *list, char *value)
{
	int ind;
	char text[64];

	if (myNode==NULL) return;
  if ((ind=list->GetCurSel())>=0)
  {
		if (value)
	    list->GetLBText(ind, value);
		else
		{
			value=text;
	    list->GetLBText(ind, text);
		}
	  myNode->put(name, value);
	}
	else
	{
	  myNode->remove(name);
	}
}

BSTR ModuleDlg::AsciiToBSTR(const char* pszFName)
{
   WCHAR wszURL[4096];
   ::MultiByteToWideChar(CP_ACP, 0, pszFName, -1, wszURL, 4096);
   return SysAllocString(wszURL);
}

void ModuleDlg::SetDialog(void *dlg)
{
  mainDlg = dlg;
}

void ModuleDlg::SetNode(HashtableExt *node)
{
  myNode = node;
}

void ModuleDlg::SetSize(int w, int h)
{
  width=w;
  height=h;
}

void ModuleDlg::SetDimension(int x, int y, int w, int h)
{
  left=x;
	top=y;
	width=w;
  height=h;
}

void ModuleDlg::RefreshValues()
{
}

void ModuleDlg::SaveValues()
{
}

void ModuleDlg::LoadValues()
{
}

int ModuleDlg::Process(unsigned char *data, short int *tmpImage, unsigned char *tmpImage2, int left, int top, int width, int height)
{
  return 0;
}

void ModuleDlg::Annotate(unsigned char *data, short int *tmpImage, unsigned char *tmpImage2, int left, int top, int width, int height)
{
}

bool ModuleDlg::Initialize()
{
	return true;
}

void ModuleDlg::Destroy()
{
}

void ModuleDlg::SetVariable(char *name, int i)
{
  char buffer[16];
  sprintf(buffer, "%d", i);
  global->put(name, buffer);
}

void ModuleDlg::SetVariable(char *name, float f)
{
  char buffer[32];
  sprintf(buffer, "%f", f);
  global->put(name, buffer);
}

void ModuleDlg::SetVariable(char *name, int *arr)
{
  global->put(name, (void *)arr, 4, ARRAY_CONST_VALUE);
}

void ModuleDlg::SetVariable(char *name, char *val)
{
  global->put(name, val);
}

void ModuleDlg::SetVariable(char *name, CString *val)
{
  global->put(name, (char *)((LPCTSTR)val));
}

char *ModuleDlg::GetVariable(char *name)
{
  return (char *)global->get(name);
}

void ModuleDlg::SetVariableTable(HashtableExt *g)
{
  global=g;
}

void ModuleDlg::UpdateData(bool mode)
{
  CDialog::UpdateData(mode);
  if (mainDlg) ((RoboRealmExt *)mainDlg)->UpdateImage();
}

void ModuleDlg::UpdateImage()
{
  if (mainDlg) ((RoboRealmExt *)mainDlg)->UpdateImage();
}

void ModuleDlg::OnHelp(char *name)
{
	if (strnicmp(name, "http", 4)!=0)
	{
	  char buffer[256];
		_snprintf(buffer, 256, "http://www.roborealm.com/help/%s", name);
	  if (mainDlg) ((RoboRealmExt *)mainDlg)->showUrl(buffer);
	}
	else
	{
	  if (mainDlg) ((RoboRealmExt *)mainDlg)->showUrl(name);
	}
}

void ModuleDlg::OnOK()
{
	isActive = false;
  UpdateData(TRUE);

  SaveValues();

  ((RoboRealmExt *)mainDlg)->setDisplay(myNode, GetDisplay());

  CDialog::OnOK();
	isActive = true;
}

void ModuleDlg::OnCancel()
{
	isActive = false;

	CDialog::OnCancel();

  if (isInsert)
  {
    ((RoboRealmExt *)mainDlg)->remove(myNode, GetDisplay());
  }
  else
  {
    LoadValues();
		isActive = true;
		UpdateImage();
  }
}

int ModuleDlg::GetIDD()
{
  return IDD;
}

bool ModuleDlg::GetBool(char *name)
{
  char *txt = (char *)myNode->get(name);
  if (txt==NULL)
  	return FALSE;
  else
  {
	  if (isalpha(txt[0]))
    {
      if (stricmp(txt, "true")==0) return TRUE;
      if (stricmp(txt, "false")==0) return FALSE;
      if (stricmp(txt, "on")==0) return TRUE;
      if (stricmp(txt, "off")==0) return FALSE;
      if (stricmp(txt, "enabled")==0) return TRUE;
      if (stricmp(txt, "disabled")==0) return FALSE;
      if (stricmp(txt, "active")==0) return TRUE;
      if (stricmp(txt, "inactive")==0) return FALSE;
      return FALSE;
    }

    return (atoi(txt)>=1);
  }
}

void ModuleDlg::SetBool(char *name, bool value)
{
  if (value)
    myNode->put(name, "TRUE");
  else
    myNode->put(name, "FALSE");
}

void ModuleDlg::SetBool(char *name, BOOL value)
{
  if (value!=0)
    myNode->put(name, "TRUE");
  else
    myNode->put(name, "FALSE");
}

void ModuleDlg::RemoveVariable(char *name)
{
  if (global) global->remove(name);
}

int ModuleDlg::getImageNumber()
{
  if (mainDlg)
		return ((RoboRealmExt *)mainDlg)->getImageNumber();
	else
		return 0;
}

int ModuleDlg::SetVariableSelect(CComboBox *list, char *tmp)
{
  CString str;
  int i;

  if (tmp[0])
  {
    for (i=0;i<list->GetCount();i++)
    {
      list->GetLBText(i, str);
      if (strcmp((char *)((LPCTSTR)str), tmp)==0)
      {
        list->SetCurSel(i);
        i=1000;
      }
    }
    if (i<=1000)
    {
      list->SetCurSel(list->AddString(tmp));
	    return list->GetCount()-1;
    }
  }

  return 0;
}

void ModuleDlg::AddVariable(CComboBox *list, char *s)
{
  int i;
  CString str;

  for (i=1;i<list->GetCount();i++)
  {
  	list->GetLBText(i, str);
	  if (strcmp((char *)((LPCTSTR)str), s)==0) i=1000;
  }
  if (i<=1000) list->AddString(s);
}

void ModuleDlg::SetArray(char *name, int *vals, int num)
{
	int i;
	CString buffer;
	if (myNode==NULL) return;
	if (vals==NULL) return;
	if (num==0)
	  myNode->remove(name);
	else
	{
		char temp[16];
		for (i=0;i<num;i++)
		{
			if (i>0) buffer+=",";
			itoa(vals[i], temp, 10);
			buffer+=temp;
		}
	  myNode->put(name, (char *)((LPCTSTR)buffer));
	}
}

int ModuleDlg::GetArray(char *name, int *vals, int max)
{
	if (myNode==NULL) return 0;
	if (vals==NULL) return 0;
  char *txt = (char *)myNode->get(name);
	if (txt==NULL) return 0;

	int i,j;
	int last=0;
	for (j=i=0;txt[i];i++)
	{
		if ((txt[i]==',')&&(j<max))
		{
			vals[j++]=atoi(&txt[last]);
			last=i+1;
		}
	}
	if (j<max) vals[j++]=atoi(&txt[last]);
	return j;
}

int ModuleDlg::skipSubComponent()
{
  if (mainDlg)
		return ((RoboRealmExt *)mainDlg)->skipSubComponent();
	else
		return 0;
}

BOOL ModuleDlg::ShowWindow(int cmd)
{
	return CDialog::ShowWindow(cmd);
}
