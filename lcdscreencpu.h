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
  virtual keyType secTimer(struct tm *result);
  virtual keyType keyEventHandler( keyType key );
};

#endif // LCDSCREENCPU_H
