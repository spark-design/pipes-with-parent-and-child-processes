/*
Student Name: Connor Sparkman
Student NetID: cps260
Complier Used: Linux GCC
Program Description: transfers string data entered in the parent to the child over a pipe.
 */
 // Started with program 4 solution given on canvas and modified it
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#define DEBUG
pid_t cPid;
// here a few convenience functions to make the code easier
int checkCall(int val, const char *msg)
{
  if (val == -1)
    {
      if (errno == EINTR) return val; // we were interrupted, so
                                      // no reason to die here
      perror(msg);
      exit(EXIT_FAILURE);
    }
  return val;
}
// I have chosen to write the random number processing code here
// this makes it easier to do either the itimer or nanosleep
// now let's do our handlers
void pSigHnd(int sig)
{
  char buf[1024];
  pid_t p;
  // signals to handle in parent:
  // SIGCHLD
  // SIGINT
  // SIGUSR1
  // SIGUSR2
  if (sig == SIGCHLD)
    {
      while ((p = waitpid(-1, NULL, WNOHANG)) > 0);
      if (p == -1)
{
  if (errno == ECHILD)
    {
      exit(EXIT_SUCCESS);
    } else
    {
      checkCall(-1, "waitpid");
    }
}
    }
}
void cSigHnd(int sig)
{
  // signals to handle in the child:
  // SIGTERM
  // SIGALRM
  if (sig == SIGTERM)
    {
      exit(EXIT_SUCCESS);
    }
}
// function main
int main(int argc, char *argv[])
{
  // declare my variables
  char input[20], text[20];
  int fd1[2];
  if(pipe(fd1) == -1)
  {
  	perror("pipe fail");
  	return 1;	
  }
  struct sigaction pSA;
                         // had I opted to use nanosleep
  pSA.sa_handler = pSigHnd;
  sigemptyset(&pSA.sa_mask);
  pSA.sa_flags = 0;
  
  
  // now, we need to change the disposition for
  // SIGCHLD before we create a child. This is a precaution.
  checkCall(sigaction(SIGCHLD, &pSA, NULL), "sigaction");
  // all of our setup is complete
  cPid = checkCall(fork(), "fork");
  switch (cPid)
    {
    case 0:
      // in the child
      close(fd1[1]); // close the write end of the pipe
      read(fd1[0], text, 100); // read the information 
      printf("child: %s", text); 
      exit(EXIT_SUCCESS);
      while (1) pause(); // start the pause loop in the child
      break;
    default:
      // in the parent
      // add the exit handler
      close(fd1[0]); // close the read end of the pipe
      printf("Please enter text at the parent: prompt\n");
      printf("parent: \n");
      checkCall(read(STDIN_FILENO, input, 1024), "read");
      write(fd1[1], input, strlen(input) + 1); // write to the write end of the pipe
      while (1)
{
  char buf[1024];
  int numRead, numWrite;
  numRead = checkCall(read(STDIN_FILENO, buf, 1024), "read");
  if (numRead == -1) continue;
  while (1)
    {
      numWrite = checkCall(write(STDOUT_FILENO, buf, numRead), "write");
      if (numWrite == -1) continue; else break;
    }
}
    }
  
  exit (EXIT_SUCCESS);
}
