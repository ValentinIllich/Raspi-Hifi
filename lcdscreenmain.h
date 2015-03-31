// Filename:    lcdmainscreen.h
// Description: main screen and user input for record and play
//
// Open Source Licensing GPL 3
//
// Author:      Dr. Valentin Illich, www.valentins-qtsolutions.de
//--------------------------------------------------------------------------------------------------

#ifndef LCDSCREENMAIN_H
#define LCDSCREENMAIN_H

#include "lcdscreen.h"

typedef enum
{
  eNoAction,

  eNoShutdown,
  eShutDown,
  eSelection
} msgType;

class lcdscreenmain : public lcdscreen
{
public:
  lcdscreenmain();
  virtual ~lcdscreenmain();

  void startRecording();
  void stopRecording();
  void startPlay(char *playfile);
  void stopPlay();

protected:
  virtual void activatedHandler();
  virtual keyType secTimerHandler(struct tm *result);
  virtual keyType keyEventHandler( keyType key );

private:
  char m_recordfile[1024];

  int m_recordId;
  int m_playId;

  int m_startTime;

  msgType m_lastScreen;
};

#endif // LCDSCREENMAIN_H
