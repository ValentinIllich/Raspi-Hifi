#ifndef LCDSCREENMAIN_H
#define LCDSCREENMAIN_H

#include "lcdscreen.h"

class lcdscreenmain : public lcdscreen
{
public:
  lcdscreenmain();
  virtual ~lcdscreenmain();

protected:
  virtual void secTimer(struct tm *result);
  virtual int keyEventHandler( keyType key );

private:
  void startRecording();
  void stopRecording();
  void startPlay(char *playfile);
  void stopPlay();

  char m_recordfile[1024];

  int m_recorId;
  int m_playId;

  int m_startTime;
};

#endif // LCDSCREENMAIN_H
