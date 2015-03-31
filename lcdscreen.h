// Filename:    lcdscreen.h
// Description: base class for generic screen diaplay and user input
//
// Open Source Licensing GPL 3
//
// Author:      Dr. Valentin Illich, www.valentins-qtsolutions.de
//--------------------------------------------------------------------------------------------------

#ifndef LCDSCREEN_H
#define LCDSCREEN_H

#include <time.h>
#include <map>
#include <list>

#define DEFAULT_SCREEN_ID   0

#define	PIN_SWITCHER_OUT	9

extern "C"
{
#include "std_c.h"
#include "bcm2835.h"
#include "raspilcd.h"
#include "lcd.h"
}

class lcdscreen;
typedef std::map<int,lcdscreen*> screenmap;
typedef std::list<int> screenlist;

typedef enum
{
  eNone,

  eText
} objectType;

typedef struct objectinfo
{
  objectType      object;
  bool            visible;
  int             x;
  int             y;
  int             w;
  int             h;
  int             fontSize;
  const char*     text;
} objectinfo;

typedef enum
{
  eKeyNone,

  eKeyA,
  eKeyB,
  eKeyC,
  eKeyD,

  eKeyUp,
  eKeyDown,

  eKeyNext,
  eKeyPrev,
  eKeyCancel
} keyType;

typedef enum
{
  eOn,
  eOff,
  eToggle
} backlightState;

class lcdscreen
{
public:
  lcdscreen( objectinfo *objects );
  virtual ~lcdscreen();

  // to be called by main
  static void updateDisplay();
  static void updateTimer();
  static keyType keyPressed( keyType key );

  static void setupScreen( int id, lcdscreen *screen );

  static void activatePrevious();
  static void activateScreen( int id );
  static lcdscreen *getScreen( int id );

  static void setBacklightState( backlightState state );
  static void setDebuMode( bool on );

  static int toTimeInSeconds( struct tm *result );
  static struct tm *fromTimeInSeconds( int seconds );

protected:
  // to be overridden
  virtual void activatedHandler();
  virtual void deactivatedHandler();
  virtual keyType secTimerHandler(struct tm *result);
  virtual keyType keyEventHandler( keyType key );
  virtual void paintEvent();

  // to be called by derived classes
  virtual void repaint();

  // info methods
  virtual bool isActive();

private:
  static void activateScreen( lcdscreen *screen );

  virtual void activated();
  virtual void deactivated();
  virtual void drawContents();
  virtual void timetick();
  virtual keyType keyEvent( keyType key );

  // called internally
  virtual void handleKeyEvent( keyType key );

  void DemoGridIncDec( int df, int dx, int dy );
  void dumpObjectList();

  objectinfo *m_objectList;

  int m_lastTime;
  int m_ticks;
  bool m_repaint;
  bool m_isActive;

  static bool m_debugMode;
  static screenmap m_screens;
  static screenlist m_screenQueue;

  static int m_activeId;
  static lcdscreen *m_activeScreen;
};

#endif // LCDSCREEN_H
