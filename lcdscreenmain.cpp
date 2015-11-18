// Filename:    lcdmainscreen.cpp
// Description: main screen and user input for record and play
//
// Open Source Licensing GPL 3
//
// Author:      Dr. Valentin Illich, www.valentins-qtsolutions.de
//--------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include "lcdscreenmain.h"
#include "lcdscreentimer.h"
#include "lcdscreenselect.h"
#include "lcdscreencpu.h"
#include "lcdscreenmessages.h"
#include "screenids.h"

#ifdef  QT_EMULATION
int spawn (const char* /*program*/, const char** /*arg_list*/)
{
    return 0;
}
#else
#include <sys/wait.h>

int spawn (const char* program, const char** arg_list)
{
  pid_t child_pid;
  /* Duplicate this process. */
  child_pid = fork ();
  if (child_pid != 0)
    /* This is the parent process. */
    return child_pid;
  else
  {
    /* Now execute PROGRAM, searching for it in the path. */
    execvp (program, (char**)arg_list);
    /* The execvp function returns only if an error occurs. */
    fprintf (stderr, "an error occurred in execvp\n");
    _exit (-1);
  }
}
#endif

static objectinfo strings[] = {
  { eText,true,  7, 0,  0, 0,  0,"31.01.2015 12:00:39" },
  { eText,true,  0,56,  0, 0,  0,"Power           About" },
  { eText,true, 48,56,  0, 0,  0,"Timer" },
  { eText,true, 80, 9,  0, 0,  1,"Record" },
  { eText,true, 96,38,  0, 0,  1,"Play" },
  { eText,true, 17,24,  0, 0,  3,"--:--:--" },

  { eNone,false, 0,0,0,0,  0,NULL },
};

static lcdscreenmain mainscreen;

lcdscreenmain::lcdscreenmain()
  : lcdscreen(strings)
  , m_recordId(-1)
  , m_playId(-1)
  , m_startTime(-1)
  , m_lastScreen(eNoAction)
{
  strcpy(m_recordfile,"");
  setupScreen(RECORD_PLAY_SCREEN,this,"playScreen");
}

lcdscreenmain::~lcdscreenmain()
{
}

int proc_exists(pid_t pid)
{
#ifdef  QT_EMULATION
  return pid;
#else
  // this is a better method...
  if( kill(pid,0)==0 || errno==EPERM )
    return 1;
  else
    return 0;
#endif
}

void lcdscreenmain::activatedHandler()
{
  strings[2].visible=true;
  strings[3].visible=true;
  strings[4].visible=true;
  updateRemaining();
}

keyType lcdscreenmain::secTimerHandler(struct tm *result)
{
  static char buffer[128];
  static int secs = 0;

  if( m_recordId>=0 )
  {
    if( !proc_exists(m_recordId) )
    {
      myprintf("record no longer running: %d!\n",errno);
      stopRecording();
    }
  }
  if( m_playId>=0 )
  {
    if( !proc_exists(m_playId) )
    {
      myprintf("play no longer running: %d!\n",errno);
      stopPlay();
    }
  }

  int actTime = lcdscreen::toTimeInSeconds(result);
  int delta = actTime-m_startTime;

  int dh = delta/3600;
  delta = delta % 3600;
  int dm = delta / 60;
  delta = delta % 60;
  int ds = delta;

  if( lcdscreentimer::timerActive() )
  {
    if( (ds%2)==0 )
      strings[2].visible=true;
    else
      strings[2].visible=false;
  }

  if( (m_recordId>=0) || (m_playId>=0) )
  {
    static char buffer[128];

    if( (ds%2)==0 )
    {
      if( m_recordId>=0 ) strings[3].visible=true;
      if( m_playId>=0 ) strings[4].visible=true;
    }
    else
    {
      if( m_recordId>=0 ) strings[3].visible=false;
      if( m_playId>=0 ) strings[4].visible=false;
    }

    sprintf(buffer,"%02d:%02d:%02d",dh,dm,ds);
    strings[5].text = buffer;
  }

  sprintf(buffer,"%02d.%02d.%04d %02d:%02d:%02d",result->tm_mday,result->tm_mon+1,result->tm_year+1900,result->tm_hour,result->tm_min,result->tm_sec);
  strings[0].text = buffer;
  repaint();

  if( (secs%60)==0 )
  {
    FILE *fp = fopen("/home/pi/usbstick/cputemp.csv","a");
    if( fp ) fprintf(fp,"%s;%s\n",buffer,lcdscreencpu::getTemp());
    fclose(fp);
  }
  secs++;

  return eKeyNone;
}

keyType lcdscreenmain::keyEventHandler( keyType key )
{
  keyType ret = eKeyNone;

  switch(key)
  {
  case eKeyUp:
    if( m_recordId>=0 )
      stopRecording();
    else
      startRecording();
    break;
  case eKeyDown:
    if( m_playId>=0 )
    {
      stopPlay();
      strcpy(m_recordfile,"");
    }
    else if( strlen(m_recordfile)>0 )
      startPlay(m_recordfile);
    else
    {
      m_lastScreen = eSelection;
      lcdscreen::activateScreen(SELECT_SCREEN);
    }
    break;
  case eKeyA:
    if( lcdscreentimer::timerActive() || m_recordId>=0 || m_playId>=0 )
    {
      m_lastScreen = eNoShutdown;
      lcdscreen::activateScreen(POWER_DENIED);
    }
    else
    {
      m_lastScreen = eShutDown;
      lcdscreenQuestion::setMessage("  Power Down ?");
      lcdscreen::activateScreen(MESSAGE_SCREEN);
    }
    break;
  case eKeyB:
    lcdscreen::activateScreen(TIMER_SCREEN);
    break;
  case eKeyC:
    lcdscreen::activateScreen(ABOUT_SCREEN);
    break;
  case eKeyCancel:
    switch( m_lastScreen )
    {
    case eSelection:
      if( strlen(lcdscreenselect::selectedFile())>0 )
      {
        startPlay(lcdscreenselect::selectedFile());
        lcdscreenselect::resetSelectedFile();
      }
      break;
    case eShutDown:
      if( lcdscreenQuestion::YesClicked() )
      {
        lcdscreenQuestion::setMessage("  Shut Down...");
        lcdscreenQuestion::setButtons("");
        lcdscreen::activateScreen(MESSAGE_SCREEN);
        system("sudo umount /dev/sda1");
        system("sudo shutdown -h now");
      }
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }

  myprintf("mainscreen returning %s\n",lcdscreen::keyToString(ret));
  return ret;
}

void lcdscreenmain::updateRemaining()
{
  int hours=0,mins=0,secs=0;
  lcdscreenselect::getRemainig(hours,mins,secs);
  sprintf(m_remaining,"%02d:%02d:%02d",hours,mins,secs);
  strings[5].text = m_remaining;
}

void lcdscreenmain::startRecording()
{
  if( m_recordId==-1 )
  {
    time_t clock = time(NULL);
    struct tm *result = localtime(&clock);
    sprintf(m_recordfile,"/home/pi/usbstick/%04d-%02d-%02dT%02d-%02d-%02d.wav",result->tm_year+1900,result->tm_mon+1,result->tm_mday,result->tm_hour,result->tm_min,result->tm_sec);

    const char *args[] = { "/usr/bin/arecord","-f", "cd", "-Dhw:1,0", "-r", "44100", m_recordfile, NULL };
    m_recordId = spawn("/usr/bin/arecord",args);
    myprintf("record process id is %d\n",m_recordId);

    m_startTime = lcdscreen::toTimeInSeconds(result);
    strings[5].text = "00:00:00";
    repaint();
  }
}

void lcdscreenmain::stopRecording()
{
  if( m_recordId>=0 )
  {
#ifndef  QT_EMULATION
    int ret = kill(m_recordId,SIGTERM);
    printf("record process killed: %d\n",ret);
#endif
    m_recordId = -1;
    strings[3].visible=true;
    updateRemaining();
    repaint();
  }
}

void lcdscreenmain::startPlay(char *playfile)
{
  if( m_playId==-1 )
  {
    time_t clock = time(NULL);
    struct tm *result = localtime(&clock);
    const char *args[] = { "/usr/bin/aplay","-f", "cd", "-Dhw:1,0", "-r", "44100", playfile, NULL };
    m_playId = spawn("/usr/bin/arecord",args);
    myprintf("play process id is %d\n",m_playId);

    m_startTime = lcdscreen::toTimeInSeconds(result);
    strings[5].text = "00:00:00";
    repaint();
  }
}

void lcdscreenmain::stopPlay()
{
  if( m_playId>=0 )
  {
#ifndef  QT_EMULATION
    int ret = kill(m_playId,SIGTERM);
    printf("play process killed: %d\n",ret);
    m_playId = -1;
#endif
    strings[4].visible=true;
    updateRemaining();
    repaint();
  }
}
