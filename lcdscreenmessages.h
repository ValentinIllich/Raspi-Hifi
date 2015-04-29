// Filename:    lcdscreenmessages.h
// Description: showing shutdown message / ignore warning
//
// Open Source Licensing GPL 3
//
// Author:      Dr. Valentin Illich, www.valentins-qtsolutions.de
//--------------------------------------------------------------------------------------------------

#ifndef LCDSCREENMESSAGES_H
#define LCDSCREENMESSAGES_H

#include "lcdscreen.h"

class lcdscreenmessages : public lcdscreen
{
public:
  lcdscreenmessages( objectinfo *objects );
  ~lcdscreenmessages();

protected:
  virtual void activatedHandler();
  virtual keyType secTimerHandler(struct tm *result);

private:
  int m_startingSeconds;
};

class lcdscreenShutdown : public lcdscreenmessages
{
public:
  lcdscreenShutdown();
  ~lcdscreenShutdown();

protected:
  virtual keyType keyEventHandler( keyType key );
};

class lcdscreenNoShutDown : public lcdscreenmessages
{
public:
  lcdscreenNoShutDown();
  ~lcdscreenNoShutDown();

protected:
  virtual keyType keyEventHandler( keyType key );
};

#endif // LCDSCREENMESSAGES_H
