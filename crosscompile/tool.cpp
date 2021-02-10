/*
    cross compile tool: this will be a replacement for compiler and linker in QMAKE settings
    also used for starting the application on remote device
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

volatile bool running = true;

typedef void (*sighandler_t)(int);
extern "C" sighandler_t signal(int signum, sighandler_t handler);

void terminateHandler()
{
  running = false;
}

int main( int argc, char **argv )
{
    signal(SIGTERM, (sighandler_t)terminateHandler);
    // avoid zombie processes...
    if (signal(SIGCHLD, SIG_IGN) == SIG_ERR)
    {
      perror(0);
      exit(1);
    }

    char *remote_target_path = getenv("REMOTE_PATH");
    char *remote_obj_path = getenv("REMOTE_OBJ_PATH");
    char *remote_login = getenv("REMOTE_DEVICE_ID");

    bool compiling = false;
    static char command[4096];
    const char *outputfile = "";

    int i=0;
    if( strcmp(argv[1],"-r")==0 )
    {
      // run step
      printf("executing...\n");

      sprintf(command,"ssh -f %s \"sudo %s/%s/Raspi-Hifi-V1\"",remote_login,remote_target_path,remote_obj_path);
      system(command);

      while( running )
        i++;

      sprintf(command,"ssh -f %s \"sudo killall -SIGTERM Raspi-Hifi-V1\"",remote_login);
      system(command);

      printf("...done\n");

      return 0;
    }

    if( strcmp(argv[1],"-c")==0 )
    {
      // compile step
      compiling = true;

      sprintf(command,"/usr/bin/rsync -qavzhe ssh %s %s:%s",argv[argc-1],remote_login,remote_target_path);
      system(command);

      if( strstr(argv[argc-1],".cpp")!=NULL )
        sprintf(command,"ssh %s \"cd %s;g++",remote_login,remote_target_path);
      else
        sprintf(command,"ssh %s \"cd %s;gcc",remote_login,remote_target_path);
    }
    else
    {
      // link step
      sprintf(command,"ssh %s \"cd %s;g++",remote_login,remote_target_path);
    }

    for( int j=1; j<argc; j++ )
    {
      strcat(command," ");
      strcat(command,argv[j]);

      if( strcmp(argv[j],"-o")==0 )
        outputfile = argv[j+1];
    }

    strcat(command,"\"");
    system(command);

    if( compiling )
    {
      sprintf(command,"/usr/bin/rsync -qavzhe ssh %s:%s/%s %s",remote_login,remote_target_path,argv[argc-2],remote_obj_path);
      system(command);
    }
    else
    {
      sprintf(command,"/usr/bin/rsync -qavzhe ssh %s:%s/%s %s",remote_login,remote_target_path,outputfile,remote_obj_path);
      system(command);
    }

    printf("...done\n");
}
