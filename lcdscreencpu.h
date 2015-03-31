// Filename:    lcdscreencpu
// Description: shows the cpu temperature histoty
//
// Open Source Licensing GPL 3
//
// Author:      Martin Steppuhn, www.emsystech.de
// Author:      Dr. Valentin Illich, www.valentins-qtsolutions.de
//--------------------------------------------------------------------------------------------------

#ifndef LCDSCREENCPU_H
#define LCDSCREENCPU_H

#include "lcdscreen.h"

class lcdscreencpu : public lcdscreen
{
public:
  lcdscreencpu();
  virtual ~lcdscreencpu();

protected:
  virtual void paintEvent();
  virtual keyType secTimerHandler(struct tm *result);
  virtual keyType keyEventHandler( keyType key );
};

#endif // LCDSCREENCPU_H
