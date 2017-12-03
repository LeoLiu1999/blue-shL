#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define DEBUGGING 0 //Set to 1/0 as appropriate

//returns the new fileno of stdin
//changes STDIN_FILENO to target
void redirect_stdin(char** chargs, int i){
  int inFile = open(chargs[i+1], O_RDONLY);
  dup2( inFile, STDIN_FILENO);
  while (chargs[i]){
    chargs[i] = chargs[i+2];
    i++;}
}


void redirect_stdout(char** chargs, int i){
  int outFile = open(chargs[i+1], O_CREAT | O_WRONLY, 0744);
  dup2( outFile, STDOUT_FILENO);
  while (chargs[i]){
    chargs[i] = chargs[i+2];
    i++;}
}

char **parse_args(char * line, char *delimiter){
  char **ret = malloc(100);
  ret[0]=line;
  unsigned char i = 1;
  char * args;
  while( (args = strsep(&line, delimiter)))
    ret[i++] = args;
  ret[i] = 0;
  return ret;
}

void special_funcs(char ** chargs) {
  char *cmd = chargs[0];

  if (DEBUGGING) printf("Special function \"%s\" to be executed\n", cmd);

  if (!strcmp(cmd, "cd"))
    chdir(chargs[2]);
  else if (!strcmp(cmd, "exit"))
    exit(0); //sometimes doesn't work?
}

void execute_single(char * line){
  if (!line)
    return;

  // Remove extra chars
  while (*line == ' ' || *line == '\n')
    line++;

  char **chargs = parse_args(line, " ");
  if (!( strcmp(chargs[0],"cd") && strcmp(chargs[0], "exit")))
    return special_funcs(chargs);

  int f = fork(); // Fork off a child to exec commands

  if (!f) {
    int i;
    
    i = 1; // scan for redirects
    while (chargs[i]){
      if(!strcmp(chargs[i],"<")) {
        if (DEBUGGING) printf("Redirecting input of \"%s\" from \"%s\"\n", chargs[i-1], chargs[i+1]);
        redirect_stdin(chargs, i);
        break;
      }
      else if(!strcmp(chargs[i],">")) {
        if (DEBUGGING) printf("Redirecting output of \"%s\" to \"%s\"\n", chargs[i-1], chargs[i+1]);
        redirect_stdout(chargs, i);
        break;
      }
      i++;
    }

    if (DEBUGGING) { // Print debug info
      printf("Executing \"%s\" with the following parameters:\n", chargs[0]);
      i = 1;
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
    printf("All commands to be run:\n");
    while(cmds[i++])
      printf("CMDS[%d]: %s\n", i, cmds[i]);
    i = 0;
    printf("\n\n");
  }

  while (cmds[i++]){
    //int fd = open("./foo", O_CREAT | O_RDWR, 0744);
    //printf("FD: %d, ERRNO: %d\n", fd, errno);
    execute_single(cmds[i]);
  }
}

void prompt_user(char * buf) {
  char wd[100];
  printf("\e[36mblue-shL:\e[33m%s\e[37m$ ", getcwd(wd, 100));
  fgets(buf, 100, stdin);
}

int main(){
  char *s = (char *)malloc(101);
  while ( 1 ){
    prompt_user(s);
    execute(s);
  }
  free(s);
  return 0;
}
