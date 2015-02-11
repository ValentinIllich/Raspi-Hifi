#include <stdio.h>
#include <string.h>
#include <time.h>

#include "lcdscreen.h"

bool lcdscreen::m_debugMode = false;
lcdscreen *lcdscreen::m_activeScreen = 0;

lcdscreen::lcdscreen(objectinfo *objects)
  : m_objectList(objects)
  , m_repaint(false)
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
  // \todo hier evtl. alle durchgehen...
  if( m_activeScreen ) m_activeScreen->timetick();
}

int lcdscreen::keyPressed( keyType key )
{
  if( m_activeScreen )
    return m_activeScreen->keEvent(key);
  else
    return 0;
}

void lcdscreen::activateScreen( lcdscreen *screen )
{
  printf("activeScreen\n");
  m_activeScreen = screen;
  if( m_activeScreen ) m_activeScreen->repaint();
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
    unsigned int i = 0;

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

    for( i=0; m_objectList[i].object!=eNone; i++ )
    {
      if( !m_objectList[i].visible )
        continue;

      if( editText==-1 || editText==i )
      {
        LCD_SetFont(m_objectList[i].fontSize);
        LCD_PrintXY(m_objectList[i].x,m_objectList[i].y,m_objectList[i].text);
      }
    }

    LCD_WriteFramebuffer();

    m_repaint = false;
    //printf("painting...\n");
  }
}

void lcdscreen::secTimer(struct tm *result)
{
}
int lcdscreen::keyEventHandler( keyType key )
{
}
void lcdscreen::paintEvent()
{
}

void lcdscreen::timetick()
{
  static int lastTime = 0;
  static int ticks = 0;

  time_t clock = time(NULL);
  struct tm *result = localtime(&clock);
  int actTime = result->tm_sec + result->tm_min*60 + result->tm_hour*3600;

  if( actTime!=lastTime )
  {
    lastTime=actTime;
    //printf("%s\n",buffer);
    //printf("%d ticks\n",ticks);
    ticks = 0;

    // this is now a second timer...
    secTimer(result);

    // timeout for backlight is 20 secs
    if( !m_debugMode ) BacklightTimer++;
    if( BacklightTimer==60 ) setBacklightState(eOff);
  }
  else
    ticks++;
}

void lcdscreen::repaint()
{
  m_repaint = true;
  //printf("repaint()\n");
}

int lcdscreen::keEvent( keyType key )
{
  // retrigger backlight timer
  BacklightTimer = 0;

  int retval = 0;

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
  }

  printf("lcdscreen returned %d\n",retval);
  return retval;
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
