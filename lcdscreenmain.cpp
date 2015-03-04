// Filename:    lcdmainscreen.h
// Description: main screen and user input for record and play
//
// Open Source Licensing GPL 3
//
// Author:      Dr. Valentin Illich, www.valentins-qtsolutions.de
//--------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>

#include "lcdscreenmain.h"

static objectinfo strings[] = {
  { eText,true,  7, 0,  0, 0,  0,"31.01.2015 12:00:39" },
  { eText,true,  0,56,  0, 0,  0,"Power 789012345678901" },
  { eText,true, 80, 9,  0, 0,  1,"Record" },
  { eText,true, 96,38,  0, 0,  1,"Play" },
  { eText,true, 17,24,  0, 0,  3,"--:--:--" },

  { eNone,false, 0,0,0,0,  0,NULL },
};

int spawn (const char* program, const char** arg_list)
{
pid_t child_pid;
/* Duplicate this process. */
child_pid = fork ();
if (child_pid != 0)
    /* This is the parent process. */
     return child_pid;
else {
    /* Now execute PROGRAM, searching for it in the path. */
     execvp (program, (char**)arg_list);
    /* The execvp function returns only if an error occurs. */
    fprintf (stderr, "an error occurred in execvp\n");
    _exit (-1);
    }
 }

static lcdscreenmain mainscreen;

lcdscreenmain::lcdscreenmain()
  : lcdscreen(strings)
  , m_recorId(-1)
  , m_playId(-1)
  , m_startTime(-1)
{
  strcpy(m_recordfile,"");
  setupScreen(0,this);
}

lcdscreenmain::~lcdscreenmain()
{
}

int proc_exists(pid_t pid)
{
#if 0
  static char procSpace[128];
  sprintf(procSpace,"/proc/%d/maps",pid);
  if( FILE *fp=fopen(procSpace,"rb") )
  {
    fclose(fp);
    return 1;
  }
  return 0;
#else
  // this is a better method...
  if( kill(pid,0)==0 || errno==EPERM )
    return 1;
  else
    return 0;
#endif
}

keyType lcdscreenmain::secTimer(struct tm *result)
{
  static char buffer[4096];

  if( m_recorId>=0 )
  {
    if( !proc_exists(m_recorId) )
    {
      printf("record no longer running: %d!\n",errno);
      stopRecording();
    }
  }
  if( m_playId>=0 )
  {
    if( !proc_exists(m_playId) )
    {
      printf("play no longer running: %d!\n",errno);
      stopPlay();
    }
  }

  if( (m_recorId>=0) || (m_playId>=0) )
  {
    static char buffer[128];

    int actTime = result->tm_sec + result->tm_min*60 + result->tm_hour*3600;
    int delta = actTime-m_startTime;

    int dh = delta/3600;
    delta = delta % 3600;
    int dm = delta / 60;
    delta = delta % 60;
    int ds = delta;

    if( (ds%2)==0 )
    {
      if( m_recorId>=0 ) strings[2].visible=true;
      if( m_playId>=0 ) strings[3].visible=true;
    }
    else
    {
      if( m_recorId>=0 ) strings[2].visible=false;
      if( m_playId>=0 ) strings[3].visible=false;
    }

    sprintf(buffer,"%02d:%02d:%02d",dh,dm,ds);
    strings[4].text = buffer;
  }

  sprintf(buffer,"%02d.%02d.%04d %02d:%02d:%02d",result->tm_mday,result->tm_mon+1,result->tm_year+1900,result->tm_hour,result->tm_min,result->tm_sec);
  strings[0].text = buffer;
  repaint();

  return eKeyNone;
}

keyType lcdscreenmain::keyEventHandler( keyType key )
{
  keyType ret = eKeyNone;

  switch(key)
  {
  case eKeyB:
    //ret = eKeyNext;
    lcdscreen::activateScreen(1);
    break;
  case eKeyUp:
    if( m_recorId>=0 )
      stopRecording();
    else
      startRecording();
    break;
  case eKeyDown:
    if( m_playId>=0 )
      stopPlay();
    else if( strlen(m_recordfile)>0 )
      startPlay(m_recordfile);
    break;
  case eKeyA:
    system("sudo umount /dev/sda1");
    system("sudo shutdown -h now");
    break;
  default:
    break;
  }

  printf("mainscreen returned %d\n",ret);
  return ret;
}

void lcdscreenmain::startRecording()
{
  if( m_recorId==-1 )
  {
    time_t clock = time(NULL);
    struct tm *result = localtime(&clock);
    sprintf(m_recordfile,"/home/pi/usbstick/%04d-%02d-%02dT%02d-%02d-%02d.wav",result->tm_year+1900,result->tm_mon+1,result->tm_mday,result->tm_hour,result->tm_min,result->tm_sec);

    const char *args[] = { "/usr/bin/arecord","-f", "cd", "-Dhw:1,0", "-r", "44100", m_recordfile, NULL };
    m_recorId = spawn("/usr/bin/arecord",args);
    printf("record process id is %d\n",m_recorId);

    m_startTime = result->tm_sec + result->tm_min*60 + result->tm_hour*3600;
    strings[4].text = "00:00:00";
    repaint();
  }
}

void lcdscreenmain::stopRecording()
{
  if( m_recorId>=0 )
  {
    int ret = kill(m_recorId,SIGTERM);
    printf("record process killed: %d\n",ret);
    m_recorId = -1;
    strings[2].visible=true;
    strings[4].text = "--:--:--";
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
    printf("play process id is %d\n",m_playId);

    m_startTime = result->tm_sec + result->tm_min*60 + result->tm_hour*3600;
    strings[4].text = "00:00:00";
    repaint();
  }
}

void lcdscreenmain::stopPlay()
{
  if( m_playId>=0 )
  {
    int ret = kill(m_playId,SIGTERM);
    printf("play process killed: %d\n",ret);
    m_playId = -1;
    strings[3].visible=true;
    strings[4].text = "--:--:--";
    repaint();
  }
}
