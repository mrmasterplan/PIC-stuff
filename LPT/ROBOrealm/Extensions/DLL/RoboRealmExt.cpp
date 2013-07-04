#include "StdAfx.h"
#include "RoboRealmExt.h"

RoboRealmExt::RoboRealmExt(int id, CWnd* pParent)
	: CDialog(id, pParent)
{
}

void RoboRealmExt::UpdateImage()
{
}

void RoboRealmExt::UpdateData(bool mode)
{
}

void RoboRealmExt::setDisplay(HashtableExt *node, char *display)
{
}

void RoboRealmExt::remove(HashtableExt *node, char *display)
{
}

void RoboRealmExt::showUrl(char *url)
{
}

unsigned char *RoboRealmExt::setSizes(int width, int height)
{
	return NULL;
}

void RoboRealmExt::setDimensions(int x, int y, int width, int height)
{
}

int RoboRealmExt::getImageNumber()
{
	return 0;
}

int RoboRealmExt::skipSubComponent()
{
	return 0;
}

int RoboRealmExt::ProgramLoad(char *filename)
{
	return 0;
}

void RoboRealmExt::StopCamera()
{
}
