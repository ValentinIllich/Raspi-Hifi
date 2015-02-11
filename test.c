#include <time.h>
#include <stdio.h>
#include <unistd.h>

int spawn (char* program, char** arg_list)
{
pid_t child_pid;
/* Duplicate this process. */
child_pid = fork ();
if (child_pid != 0)
    /* This is the parent process. */
     return child_pid;
else {
    /* Now execute PROGRAM, searching for it in the path. */
     execvp (program, arg_list);
    /* The execvp function returns only if an error occurs. */
    fprintf (stderr, "an error occurred in execvp\n");
    abort ();
    }
 }

int main(int argc, char **argv)
{
  int ret = system("sudo /bin/mount -t vfat -o uid=pi,gid=pi /dev/sda1 /home/pi/usbstick/");
  printf("mount returned %d\n",ret);

  //system("sudo shutdown -h now");
  char *args[] = { "/usr/bin/arecord", "-f","cd","-d","600","-Dhw:1,1","-r","44100","/home/pi/usbstick/xxx.wav",NULL};
  spawn("/usr/bin/arecord",args);

  time_t clock = time(NULL);
  struct tm *result = localtime(&clock);
printf("%02d.%02d.%04d %02d:%02d:%02d\n",result->tm_mday,result->tm_mon+1,result->tm_year+1900,result->tm_hour,result->tm_min,result->tm_sec);

/*  int ret = system("sudo /bin/mount -t vfat -o uid=pi,gid=pi /dev/sda1 /home/pi/usbstick/");
  printf("mount returned %d\n",ret);

  char *args[] = { "/usr/bin/arecord","-f", "cd", "-d", "600", "-Dhw:1,1", "-r", "44100", "/home/pi/usbstick/xxx.wav", NULL };
  spawn("/usr/bin/arecord",args);

  //system("sudo shutdown -h now");

  time_t clock = time(NULL);
  struct tm *result = localtime(&clock);
printf("%02d.%02d.%04d %02d:%02d:%02d\n",result->tm_mday,result->tm_mon+1,result->tm_year+1900,result->tm_hour,result->tm_min,result->tm_sec);*/
  return 0;
}
