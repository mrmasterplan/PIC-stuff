#ifndef _HASHTABLEEXT
#define _HASHTABLEEXT 1

class HashtableExt
{
public:
  virtual bool put(char *key, char *value);
  virtual void *get(char *key);
  virtual void *get(char *key, int len);
  virtual bool put(char *key, void *value, unsigned long int len, int type);
  virtual char *firstKey();
  virtual char *nextKey();
  virtual void *value();
  virtual long valueLength();
  virtual long keyLength();
  virtual int size();
  virtual long remove(char *);
  virtual long getInt(char *key);
  virtual long getInt(char *key, int len);
};

#endif
