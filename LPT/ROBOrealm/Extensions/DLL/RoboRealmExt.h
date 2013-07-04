#ifndef _ROBOREALMEXT
#define _ROBOREALMEXT 1

#include "stdafx.h"
#include "resource.h"
#include "Windows.h"
#include "HashtableExt.h"

class RoboRealmExt : public CDialog
{

public:
	RoboRealmExt(int id, CWnd* pParent);	// standard constructor
	virtual void UpdateImage();
	virtual unsigned char *setSizes(int width, int heigth);
	virtual void setDimensions(int x, int y, int width, int heigth);
	virtual void UpdateData(bool mode);
  virtual void setDisplay(HashtableExt *node, char *display);
  virtual void remove(HashtableExt *node, char *display);
  virtual void showUrl(char *url);
  virtual int getImageNumber();
	virtual int skipSubComponent();
	virtual int ProgramLoad(char *filename);
	virtual void StopCamera();
};

#endif
