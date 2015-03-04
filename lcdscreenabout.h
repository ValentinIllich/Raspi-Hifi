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
  virtual keyType secTimer(struct tm *result);
  virtual keyType keyEventHandler( keyType key );

private:
  int m_seconds;
};

#endif // LCDSCREENABOUT_H
