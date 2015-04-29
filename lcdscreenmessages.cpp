// Filename:    lcdscreenmessages.h
// Description: showing shutdown message / ignore warning
//
// Open Source Licensing GPL 3
//
// Author:      Dr. Valentin Illich, www.valentins-qtsolutions.de
//--------------------------------------------------------------------------------------------------

#include "lcdscreenmessages.h"
#include "screenids.h"

#include <stdlib.h>

lcdscreenmessages::lcdscreenmessages( objectinfo *objects )
  : lcdscreen(objects)
  , m_startingSeconds(0)
{
}

lcdscreenmessages::~lcdscreenmessages()
{
}

void lcdscreenmessages::activatedHandler()
{
  m_startingSeconds = 0;
}

keyType lcdscreenmessages::secTimerHandler(struct tm *result)
{
  if( m_startingSeconds==0 )
    m_startingSeconds = lcdscreen::toTimeInSeconds(result);

  int actualTime = lcdscreen::toTimeInSeconds(result);

  if( (actualTime-m_startingSeconds)>10 )
    return eKeyCancel;

  return eKeyNone;
}

//////////////////////////////////////////////////////////////////////////////////////

static objectinfo strings[] = {
  { eText,true ,17,17,  0, 0, 1,"Power Down ?" },
  { eText,true,  0,56,  0, 0,  0," Yes           Cancel" },
  { eNone,false, 0,0,0,0,  0,NULL },
};

static lcdscreenShutdown shutdown;

lcdscreenShutdown::lcdscreenShutdown() : lcdscreenmessages(strings)
{
  lcdscreen::setupScreen(POWER_SCREEN,&shutdown);
}

lcdscreenShutdown::~lcdscreenShutdown()
{
}

keyType lcdscreenShutdown::keyEventHandler( keyType key )
{
  switch( key )
  {
  case eKeyA:
    strings[0].text = "Shut Down...";
    strings[1].text = "";
    repaint();
    system("sudo umount /dev/sda1");
    system("sudo shutdown -h now");
    break;
  case eKeyC:
    return eKeyCancel;
    break;
  default:
    break;
  }

  return eKeyNone;
}

//////////////////////////////////////////////////////////////////////////////////////

static objectinfo message[] = {
  { eText,true , 4,21,  0, 0, 1,"Device is Busy!" },
  { eText,true,  9,55,  0, 0, 0,"OK" },
  { eNone,false, 0,0,0,0,  0,NULL },
};

static lcdscreenNoShutDown noshutdown;

lcdscreenNoShutDown::lcdscreenNoShutDown() : lcdscreenmessages(message)
{
  lcdscreen::setupScreen(POWER_DENIED,&noshutdown);
}

lcdscreenNoShutDown::~lcdscreenNoShutDown()
{
}

keyType lcdscreenNoShutDown::keyEventHandler( keyType key )
{
  switch( key )
  {
  case eKeyA:
    return eKeyCancel;
    break;
  default:
    break;
  }

  return eKeyNone;
}
