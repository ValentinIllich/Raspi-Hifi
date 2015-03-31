// Filename:    lcdscreen.h
// Description: base class for generic screen diaplay and user input
//
// Open Source Licensing GPL 3
//
// Author:      Dr. Valentin Illich, www.valentins-qtsolutions.de
//--------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "lcdscreen.h"

bool lcdscreen::m_debugMode = false;
int lcdscreen::m_activeId = 0;
lcdscreen *lcdscreen::m_activeScreen = 0;

screenmap lcdscreen::m_screens;
screenlist lcdscreen::m_screenQueue;

lcdscreen::lcdscreen(objectinfo *objects)
  : m_objectList(objects)
  , m_lastTime(0)
  , m_ticks(0)
  , m_repaint(false)
  , m_isActive(false)
{
}

lcdscreen::~lcdscreen()
{
}

static int editText = -1;

void lcdscreen::updateDisplay()
{
  if( m_activeScreen ) m_activeScreen->drawContents();
}

void lcdscreen::updateTimer()
{
  //if( m_activeScreen ) m_activeScreen->timetick();
  screenmap::iterator it = m_screens.begin();
  while( it!=m_screens.end() )
  {
    it->second->timetick();
    ++it;
  }
}

keyType lcdscreen::keyPressed( keyType key )
{
  keyType ret = eKeyNone;

  if( m_activeScreen )
  {
    ret = m_activeScreen->keyEvent(key);
    /*    switch( ret )
    {
    case eKeyPrev:
      activatePrevious();
      ret = m_activeScreen->keyEvent(ret);
    case eKeyNext:
    case eKeyCancel:
      break;
    default:
      break;
    }*/
  }

  return ret;
}

void lcdscreen::setupScreen( int id, lcdscreen *screen )
{
  screenmap::iterator it = m_screens.find(id);
  if( it==m_screens.end() )
    m_screens[id] = screen;
}

void lcdscreen::activatePrevious()
{
  if( m_screenQueue.size()>0 )
  {
    int id = m_screenQueue.back();
    m_screenQueue.pop_back();
    screenmap::iterator it = m_screens.find(id);
    if( it!=m_screens.end() )
    {
      activateScreen(it->second);
      m_activeId = id;
    }
  }
}

void lcdscreen::activateScreen( int id )
{
  printf("activeScreen %d\n",id);
  screenmap::iterator it = m_screens.find(id);
  if( it!=m_screens.end() )
  {
    m_screenQueue.push_back(m_activeId);
    activateScreen(it->second);
    m_activeId = id;
  }
//  else
//  {
//    activateScreen((lcdscreen*)0);
//    m_activeId = -1;
//  }
}

void lcdscreen::activateScreen( lcdscreen *screen )
{
  if( m_activeScreen ) m_activeScreen->deactivatedHandler();
  m_activeScreen = screen;
  if( m_activeScreen )
  {
    m_activeScreen->activatedHandler();
    m_activeScreen->repaint();
  }
}

lcdscreen *lcdscreen::getScreen( int id )
{
  screenmap::iterator it = m_screens.find(id);
  if( it!=m_screens.end() )
    return it->second;

  return NULL;
}

static int Backlight = 1;
static int BacklightTimer = 0;

static bool m_resize = false;

void lcdscreen::setBacklightState( backlightState state )
{
  switch( state )
  {
  case eOn:
    Backlight = 1;
    break;
  case eOff:
    Backlight = 0;
    break;
  case eToggle:
    Backlight = (Backlight) ? 0 : 1;	// Toggle Backlight
    break;
  }
  SetBacklight(Backlight);			// Write to Hardware
}

void lcdscreen::setDebuMode( bool on )
{
  m_debugMode = on;
  if( m_activeScreen ) m_activeScreen->repaint();
}

void lcdscreen::drawContents()
{
  if( m_repaint )
  {
    int i = 0;

    LCD_ClearScreen();

    if( m_debugMode )
    {
      LCD_SetPenColor(1);
      for( i=0;i<128;i+=16)
      {
        LCD_DrawLine(0,i,127,i);
        LCD_DrawLine(i,0,i,64);
      }
      LCD_DrawLine(0,63,127,63);
      LCD_DrawLine(127,0,127,64);
    }

    paintEvent();

    if( m_objectList!=NULL )
    {
      for( i=0; m_objectList[i].object!=eNone; i++ )
      {
        if( !m_objectList[i].visible )
          continue;

        if( (editText==-1) || (editText==i) )
        {
          LCD_SetFont(m_objectList[i].fontSize);
          LCD_PrintXY(m_objectList[i].x,m_objectList[i].y,m_objectList[i].text);
        }
      }
    }

    LCD_WriteFramebuffer();

    m_repaint = false;
    //printf("painting...\n");
  }
}

void lcdscreen::activated()
{
  m_isActive = true;
  activatedHandler();
}

void lcdscreen::deactivated()
{
  m_isActive = false;
  deactivatedHandler();
}

void lcdscreen::activatedHandler()
{
}
void lcdscreen::deactivatedHandler()
{
}
keyType lcdscreen::secTimerHandler(struct tm */*result*/)
{
  return eKeyNone;
}
keyType lcdscreen::keyEventHandler( keyType /*key*/ )
{
  return eKeyNone;
}
void lcdscreen::paintEvent()
{
}

void lcdscreen::timetick()
{
  time_t clock = time(NULL);
  struct tm *result = localtime(&clock);
  int actTime = lcdscreen::toTimeInSeconds(result);

  if( actTime!=m_lastTime )
  {
    m_lastTime=actTime;
    //printf("%s\n",buffer);
    //printf("%d ticks\n",ticks);
    m_ticks = 0;

    // this is now a second timer...
    keyType key = secTimerHandler(result);

    if( (key!=eKeyNone) && (this==m_activeScreen) )
      handleKeyEvent(key);

    if( this==m_activeScreen )
    {
      // only the active screen may handle the timeout for backlight (60 secs)
      if( !m_debugMode ) BacklightTimer++;
      if( BacklightTimer==60 ) setBacklightState(eOff);
    }
  }
  else
    m_ticks++;
}

void lcdscreen::repaint()
{
  m_repaint = true;
  //printf("repaint()\n");
}

bool lcdscreen::isActive()
{
  return m_isActive;
}

keyType lcdscreen::keyEvent( keyType key )
{
  // retrigger backlight timer
  BacklightTimer = 0;

  keyType retval = eKeyNone;

  switch( key )
  {
  case eKeyA:
    setBacklightState(eOn);
    if( m_debugMode && (editText>=0) )
      DemoGridIncDec(0,-1,0);
    else
      retval = keyEventHandler(key);
    break;
  case eKeyB:
    setBacklightState(eOn);
    if( m_debugMode && (editText>=0)  )
      DemoGridIncDec(0,1,0);
    else
      retval = keyEventHandler(key);
    break;
  case eKeyC:
    setBacklightState(eOn);
    if( m_debugMode && (editText>=0)  )
    {
      if( m_objectList[editText].object==eText )
        DemoGridIncDec(1,0,0);
      else
        m_resize = !m_resize;
    }
    else
      retval = keyEventHandler(key);
    break;
  case eKeyD:
    // mormally: light / debug: switch selected object
    if( m_debugMode )
    {
      editText++;
      if( m_objectList[editText].object==eNone )
      {
        editText = -1;
        dumpObjectList();
      }
      repaint();
    }
    else
      setBacklightState(eToggle);
    break;
  case eKeyUp:
    setBacklightState(eOn);
    if( m_debugMode && (editText>=0)  )
      DemoGridIncDec(0,0,-1);
    else
      retval = keyEventHandler(key);
    break;
  case eKeyDown:
    setBacklightState(eOn);
    if( m_debugMode && (editText>=00)  )
      DemoGridIncDec(0,0,1);
    else
      retval = keyEventHandler(key);
    break;
  default:
    retval = keyEventHandler(key);
    break;
  }

  if( retval!=eKeyNone ) handleKeyEvent(retval);

  printf("lcdscreen returned %d\n",retval);
  return retval;
}

void lcdscreen::handleKeyEvent( keyType key )
{
  switch( key )
  {
    case eKeyPrev:
    {
      screenmap::iterator it = m_screens.find(m_activeId);
      if( it!=m_screens.end() )
      {
        --it;
        activateScreen(it->first);
      }
    }
    break;
    case eKeyNext:
    {
      screenmap::iterator it = m_screens.find(m_activeId);
      if( it!=m_screens.end() )
      {
        ++it;
        activateScreen(it->first);
      }
    }
    break;
  case eKeyCancel:
    activatePrevious();
    m_activeScreen->keyEvent(key);
    break;
  default:
    break;
  }
}

void lcdscreen::DemoGridIncDec( int df, int dx, int dy )
{
  if( editText>=0 )
  {
    m_objectList[editText].fontSize += df;
    if ( m_objectList[editText].fontSize>=4 )
      m_objectList[editText].fontSize = 0;
    if ( m_objectList[editText].fontSize<0 )
      m_objectList[editText].fontSize = 3;

    if( m_resize )
    {
      m_objectList[editText].w += dx;
      if( m_objectList[editText].w>=128 )
        m_objectList[editText].w = 0;
      if( m_objectList[editText].w<0 )
        m_objectList[editText].w = 127;
    }
    else
    {
      m_objectList[editText].x += dx;
      if( m_objectList[editText].x>=128 )
        m_objectList[editText].x = 0;
      if( m_objectList[editText].x<0 )
        m_objectList[editText].x = 127;
    }

    if( m_resize )
    {
      m_objectList[editText].h += dy;
      if( m_objectList[editText].h>=64 )
        m_objectList[editText].h = 0;
      if( m_objectList[editText].h<0 )
        m_objectList[editText].h = 63;
    }
    else
    {
      m_objectList[editText].y += dy;
      if( m_objectList[editText].y>=64 )
        m_objectList[editText].y = 0;
      if( m_objectList[editText].y<0 )
        m_objectList[editText].y = 63;
    }

    repaint();
  }
}

void lcdscreen::dumpObjectList()
{
  static const char *objT[] = { "eNone","eText" };
  for( int i=0; m_objectList[i].object!=eNone; i++ )
  {
    printf("  { %s,%s, %3d,%3d,%3d,%3d, %d,\"%s\" },\n",
           objT[m_objectList[i].object],m_objectList[i].visible ? "true " : "false",
        m_objectList[i].x,m_objectList[i].y,m_objectList[i].w,m_objectList[i].h,
        m_objectList[i].fontSize,m_objectList[i].text);
  }
}

int lcdscreen::toTimeInSeconds( struct tm *result )
{
  int ret = 0;
  ret = result->tm_sec + result->tm_min*60 + result->tm_hour*3600
      + result->tm_mday*86400 + result->tm_mon*2678400 + (result->tm_year-100)*32140800;
  return ret;
}

struct tm *lcdscreen::fromTimeInSeconds( int seconds )
{
  static struct tm retval;
  retval.tm_year = seconds / 32140800;   seconds %= 32140800;
  retval.tm_mon =  seconds / 2678400;   seconds %= 2678400;
  retval.tm_mday = seconds / 86400;   seconds %= 86400;
  retval.tm_hour = seconds / 3600;   seconds %= 3600;
  retval.tm_min =  seconds / 60;   seconds %= 60;
  retval.tm_sec =  seconds;

  retval.tm_year += 100;

  return &retval;
}
