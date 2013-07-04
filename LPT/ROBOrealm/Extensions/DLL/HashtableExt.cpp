#include "StdAfx.h"
#include <stdio.h>
#include "HashtableExt.h"

bool HashtableExt::put(char *key, char *value)
{
  return false;
}

long HashtableExt::getInt(char *key, int len)
{
  return NULL;
}

long HashtableExt::getInt(char *key)
{
  return NULL;
}

void *HashtableExt::get(char *key, int len)
{
  return NULL;
}

void *HashtableExt::get(char *key)
{
  return NULL;
}

bool HashtableExt::put(char *key, void *value, unsigned long int len, int type)
{
  return false;
}

char *HashtableExt::firstKey()
{
  return NULL;
}

char *HashtableExt::nextKey()
{
  return NULL;
}

void *HashtableExt::value()
{
  return NULL;
}

long HashtableExt::valueLength()
{
  return 0;
}

long HashtableExt::keyLength()
{
  return 0;
}

int HashtableExt::size()
{
  return 0;
}

long HashtableExt::remove(char *)
{
  return 0;
}
