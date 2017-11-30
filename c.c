#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define DEBUGGING 1 //Set to 1/0 as appropriate

char **parse_args(char * line, char *delimiter){
  char **ret = malloc(100);
  ret[0]=line;
  unsigned char i = 1;
  char * args;
  while( args = strsep(&line, delimiter))
    ret[i++]=args;
  ret[i] = 0;
  return ret;
}

void execute_single(char * line){
  if (!line)
    return;

  // Remove extra chars
  while (*line == ' ' || *line == '\n')
    line++;


  int f = fork(); // Fork off a child to exec commands
  printf("f: %d\n", f);

  if (!f) {

    char **chargs = parse_args(line, " ");

    if (DEBUGGING) { // Print debug info
      printf("Executing \"%s\" with the following parameters\n", chargs[0]);
      int i = 1;
      while (chargs[i]) {
        printf("%s\n", chargs[i]);
        i++;
      }
      printf("\n");
    }

    execvp(chargs[0], chargs + 1);
  }

  wait(&f);
}

void execute(char * line) {
  if (line)
    *strchr(line, '\n') = 0;
  else
    return;

  char **cmds = parse_args(line, ";");
  unsigned char i = 0;

  if (DEBUGGING) {
    while(cmds[i++])
      printf("CMDS[%d]: %s\n", i, cmds[i]);
    i = 0;
    printf("\n\n");
  }

  while (cmds[i++]) {
    printf("%d\n", i);
    execute_single(cmds[i]);
    printf("\n\n");
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
