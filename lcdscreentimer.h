// Filename:    lcdscreentimer
// Description: handling of up to 5 timers: setting times / doing switching / starting, stopping records
//
// Open Source Licensing GPL 3
//
// Author:      Dr. Valentin Illich, www.valentins-qtsolutions.de
//--------------------------------------------------------------------------------------------------

#ifndef LCDSCREENTIMER_H
#define LCDSCREENTIMER_H

#include "lcdscreen.h"

class lcdscreentimer : public lcdscreen
{
public:
  lcdscreentimer();
  ~lcdscreentimer();

  static bool timerActive();

protected:
  virtual void activatedHandler();
  virtual keyType secTimerHandler(struct tm *result);
  virtual keyType keyEventHandler( keyType key );
  virtual void paintEvent();

private:
//  char *substring( char *str, int start, int len );
//  void incdecChar( char *str, int pos, int delta );
  bool scantimerSetting( const char *source, int editedTime = -1 );

  static bool m_timerActive;

  bool m_updateTimerSettings;

  int m_timerStart[5];
  int m_timerStop[5];
  int m_timercount;

  bool m_lastSwitcherState;
  bool m_lastRecordState;

  int m_selectedTime;
  bool m_editMode;
};

#endif // LCDSCREENTIMER_H
