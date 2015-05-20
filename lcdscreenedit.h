// Filename:    lcdscreenedit
// Description: allows editing of a date / time data set (as string) in three different forms:
//              "15.03.2015 10:05-10:07" is start / stop of a timer read from file
//              "15.03.15 10:05-10:07"   is start / stop of a timer by edit
//              "15.03.15  10:05:00"     is system clock
//
// Open Source Licensing GPL 3
//
// Author:      Dr. Valentin Illich, www.valentins-qtsolutions.de
//--------------------------------------------------------------------------------------------------

#ifndef LCDSCREENEDIT_H
#define LCDSCREENEDIT_H

#include "lcdscreen.h"

#define editFormatStartStopLong  0 // dd.mm.yyyy hh:mm-hh:mm
#define editFormatStartStopShort 1 // dd.mm.yy hh:mm-hh:mm
#define editFotmatSettingTime    2 // dd.mm.yy  hh:mm:ss

class lcdscreenedit : public lcdscreen
{
public:
  lcdscreenedit();
  ~lcdscreenedit();

  static void setInputString( const char *text );
  static char *getInputString();
  static bool scantimerSetting(  const char *source, int srcFormat, struct tm &start, struct tm & stop );

protected:
  virtual void activatedHandler();
  virtual keyType keyEventHandler( keyType key );
  virtual void paintEvent();

private:
  char *substring( char *str, int start, int len );
  void incdecChar( char *str, int pos, int delta );

  unsigned m_selectedPos;
  char m_substring[128];

  static char m_timerTextEdit[128];
};

#endif // LCDSCREENEDIT_H
