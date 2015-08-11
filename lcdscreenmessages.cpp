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
#include <string.h>

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

  return processSecTimer(result);
}

//////////////////////////////////////////////////////////////////////////////////////

static objectinfo shutDownStrings[] = {
  { eText,true ,17,17,  0, 0, 1,"Power Down ?" },
  { eText,true,  0,56,  0, 0,  0," Yes           Cancel" },
  { eNone,false, 0,0,0,0,  0,NULL },
};

static lcdscreenShutdown shutdown;

lcdscreenShutdown::lcdscreenShutdown() : lcdscreenmessages(shutDownStrings), m_shutdown(false)
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
    m_shutdown = true;
    repaint();
    break;
  case eKeyC:
    return eKeyCancel;
    break;
  default:
    break;
  }

  return eKeyNone;
}

keyType lcdscreenShutdown::processSecTimer(struct tm */*result*/)
{
  if( m_shutdown )
  {
    system("sudo umount /dev/sda1");
    system("sudo shutdown -h now");
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

keyType lcdscreenNoShutDown::processSecTimer(struct tm */*result*/)
{
  return eKeyNone;
}

//////////////////////////////////////////////////////////////////////////////////////

static objectinfo strings[] = {
  { eText,true , 0,17,  0, 0, 1,"" },
  { eText,true,  0,56,  0, 0,  0," Yes           Cancel" },
  { eNone,false, 0,0,0,0,  0,NULL },
};

static lcdscreenQuestion question;

char lcdscreenQuestion::m_message[128] = "";
char lcdscreenQuestion::m_buttons[128] = "";
bool lcdscreenQuestion:: m_yesClicked = false;

lcdscreenQuestion::lcdscreenQuestion() : lcdscreenmessages(strings)
{
  lcdscreen::setupScreen(MESSAGE_SCREEN,&question);
}

lcdscreenQuestion::~lcdscreenQuestion()
{
}

keyType lcdscreenQuestion::keyEventHandler( keyType key )
{
  switch( key )
  {
  case eKeyA:
    m_yesClicked = true;
  case eKeyC:
    return eKeyCancel;
    break;
  default:
    break;
  }

  return eKeyNone;
}

keyType lcdscreenQuestion::processSecTimer(struct tm */*result*/)
{
  m_yesClicked = false;
  return eKeyNone;
}

void lcdscreenQuestion::setMessage(const char *message)
{
  strcpy(m_message,message);
  strings[0].text = m_message;
}

void lcdscreenQuestion::setButtons(const char *buttons)
{
  strcpy(m_buttons,buttons);
  strings[1].text = m_buttons;
}

bool lcdscreenQuestion::YesClicked()
{
  return m_yesClicked;
}
