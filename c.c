#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define DEBUGGING 0 //Set to 1/0 as appropriate

char **parse_args(char * line, char *delimiter){
  char **ret = malloc(100);
  ret[0]=line;
  unsigned char i = 1;
  char * args;
  while( args = strsep(&line, delimiter))
    ret[i++]=args;
  return ret;
}

void execute_single(char * line){
  int f = fork(); // Fork off a child to exec commands

  if (!f) {
    // Remove newline
    *strchr(line, '\n') = 0;

    char **chargs = parse_args(line, " ");

    if (DEBUGGING) { // Print debug info
      printf("Executing \"%s\" with the following parameters\n", chargs[0]);
      unsigned char i = 1;
      while (chargs[i])
        printf("%s\n", chargs[i++]);
      printf("\n");
    }

    execvp(chargs[0], chargs + 1);
  }

  wait(&f);
}

void execute(char * line) {
  char **cmds = parse_args(line, ";");
  unsigned char i = 0;
  while (cmds[i]) {
    printf("%s\n", cmds[i] );
    execute_single(cmds[i++]);

  }
}

int main(){
  char *s = (char *)malloc(101);
  while ( 1 ){
    printf("blue-shL $ ");
    fgets(s, 100, stdin);
    execute(s); //TODO Handle spaces
  }
  free(s);
  return 0;
}
