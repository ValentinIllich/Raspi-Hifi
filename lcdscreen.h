#ifndef LCDSCREEN_H
#define LCDSCREEN_H

#include <time.h>

extern "C"
{
#include "std_c.h"
#include "bcm2835.h"
#include "raspilcd.h"
#include "lcd.h"
}

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
  eKeyA,
  eKeyB,
  eKeyC,
  eKeyD,

  eKeyUp,
  eKeyDown
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
  static int keyPressed( keyType key );

  static void activateScreen( lcdscreen *screen );
  static void setBacklightState( backlightState state );

  static void setDebuMode( bool on );

protected:
  // to be overridden
  virtual void secTimer(struct tm *result);
  virtual int keyEventHandler( keyType key );
  virtual void paintEvent();

  // to be called by derived classes
  virtual void repaint();

private:
  virtual void drawContents();
  virtual void timetick();
  virtual int keEvent( keyType key );

  void DemoGridIncDec( int df, int dx, int dy );
  void dumpObjectList();

  objectinfo *m_objectList;

  bool m_repaint;

  static bool m_debugMode;
  static lcdscreen *m_activeScreen;
};

#endif // LCDSCREEN_H
