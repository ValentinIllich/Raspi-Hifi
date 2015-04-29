// Filename:    lcdscreenselect.h
// Description: retreiving a wav file list and selecting a file out of this list
//
// Open Source Licensing GPL 3
//
// Author:      Dr. Valentin Illich, www.valentins-qtsolutions.de
//--------------------------------------------------------------------------------------------------

#ifndef LCDSCREENSELECT_H
#define LCDSCREENSELECT_H

#include "lcdscreen.h"

class lcdscreenselect : public lcdscreen
{
public:
  lcdscreenselect();
  ~lcdscreenselect();

  static char *selectedFile();
  static void resetSelectedFile();

protected:
  virtual void activatedHandler();
  virtual keyType secTimerHandler(struct tm *result);
  virtual keyType keyEventHandler( keyType key );
  virtual void paintEvent();

private:
  void cleanup();

  char *m_files[1024];
  int m_filecount;
  int m_startIdx;
  int m_selIdx;

  static char m_selected[1024];
};

#endif // LCDSCREENSELECT_H
