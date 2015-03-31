// Filename:    lcdscreenabout
// Description: shows program information screen
//
// Open Source Licensing GPL 3
//
// Author:      Dr. Valentin Illich, www.valentins-qtsolutions.de
//--------------------------------------------------------------------------------------------------

#ifndef LCDSCREENABOUT_H
#define LCDSCREENABOUT_H

#include "lcdscreen.h"

class lcdscreenabout : public lcdscreen
{
public:
  lcdscreenabout();
  virtual ~lcdscreenabout();

protected:
  virtual void paintEvent();
  virtual keyType secTimerHandler(struct tm *result);
  virtual keyType keyEventHandler( keyType key );

private:
  int m_seconds;
  int m_contrast;
};

#endif // LCDSCREENABOUT_H
