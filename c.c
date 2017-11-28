#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

char **parse_args(char * line){
  char **ret = malloc(100);
  ret[0]=line;
  unsigned char i = 1;
  char * args;
  while( args = strsep(&line, " "))
    ret[i++]=args;
  return ret;
}

void execute(char * line){
  char **chargs = parse_args(line);
  execvp(chargs[0], chargs);
}

int main(){
  char *s = (char *)malloc(101);
  while ( 1 ){
    fgets(s, 100, stdin);
    execute(s);
  }
  free(s);
  return 0;
}
