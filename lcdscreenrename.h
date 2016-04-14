// Filename:    lcdscreenedit
// Description: allows editing of a date / time data set (as string) in three different forms:
//              "15.03.2015 10:05-10:07" is start / stop of a timer read from file
//              "15.03.15 10:05-10:07"   is start / stop of a timer by edit
//              "15.03.15  10:05:00"     is system clock
//
// Open Source Licensing GPL 3
//
// Author:      Dr. Valentin Illich, www.valentins-qtsolutions.de
//--------------------------------------------------------------------------------------------------

#ifndef LCDSCREENRENAME_H
#define LCDSCREENRENAME_H

#include "lcdscreen.h"

class lcdscreenRename : public lcdscreen
{
public:
  lcdscreenRename();
  ~lcdscreenRename();

  void setName(char *name);

  bool SetClicked();
  char *getNewName();

protected:
  virtual keyType secTimerHandler(struct tm *result);
  virtual keyType keyEventHandler( keyType key );
  virtual void paintEvent();

private:
  int m_startingSeconds;
  bool m_entered;
  int m_edit_col;

  const char *m_actrow;

  char m_currentInput[32];
};

#endif // LCDSCREENRENAME_H
