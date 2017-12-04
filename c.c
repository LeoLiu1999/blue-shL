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

/**
 * Changes STDIN_FILENO to target, where chargs[i+1] is the target
 * Inputs:
 *   chargs: All of the commands to be executed
 *   i: The index of the character '<' in chargs
 **/
void redirect_stdin(char **chargs, int i){
  int inFile = open(chargs[i+1], O_RDONLY);
  dup2( inFile, STDIN_FILENO);
  while (chargs[i]){ // Adjusts input as necessary
    chargs[i] = chargs[i+2];
    i++;}
}


/**
 * Changes STDOUT_FILENO to target, where chargs[i+1] is the target
 * Inputs:
 *   chargs: All of the commands to be executed
 *   i: The index of the character '>' in chargs
 **/
void redirect_stdout(char **chargs, int i){
  int outFile = open(chargs[i+1], O_CREAT | O_WRONLY, 0744);
  dup2( outFile, STDOUT_FILENO);
  while (chargs[i]){ // Adjusts input as necessary
    chargs[i] = chargs[i+2];
    i++;}
}


/**
 * Parses "line" as a command, separating on delimiter
 * Inputs:
 *   line: A line of user input
 *   delimiter: The character to use for strsep
 * Outputs:
 *   An array of strings representing tokens of user input
 **/
char **parse_args(char *line, char *delimiter){
  char **ret = malloc(100);
  ret[0] = line;
  unsigned char i = 1;
  char * args;
  while( (args = strsep(&line, delimiter)))
    ret[i++] = args;
  ret[i] = 0;
  return ret;
}


/**
 * Handles shell builtins (cd & exit)
 * Inputs:
 *   chargs: All the commands to be executed
 **/
void special_funcs(char ** chargs) {
  char *cmd = chargs[0];

  if (DEBUGGING) printf("Special function \"%s\" to be executed\n", cmd);

  if (!strcmp(cmd, "cd"))
    chdir(chargs[2]);
  else if (!strcmp(cmd, "exit"))
    exit(0); //sometimes doesn't work?
}


/**
 * Executes a single bash command
 * Inputs:
 *   line: The command to be executed
 **/
void execute_single(char * line){
  // handle strange inputs
  if (!line)
    return;
  while (*line == ' ' || *line == '\n')
    line++;

  char **chargs = parse_args(line, " ");
  if (!( strcmp(chargs[0],"cd") && strcmp(chargs[0], "exit")))
    return special_funcs(chargs);

  int f = fork(); // Fork off a child to exec commands
  if (!f) { // Child runs the code
    int i;

    i = 1;
    while (chargs[i]){ // Scan for redirects
      if(!strcmp(chargs[i],"<")) { // Redirect STDIN
        if (DEBUGGING) printf("Redirecting input of \"%s\" from \"%s\"\n", chargs[i-1], chargs[i+1]);
        redirect_stdin(chargs, i--);
      }
      else if(!strcmp(chargs[i],">")) { // Redirect STDOUT
        if (DEBUGGING) printf("Redirecting output of \"%s\" to \"%s\"\n", chargs[i-1], chargs[i+1]);
        redirect_stdout(chargs, i--);
      }
      else
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
  free(chargs); // USA! USA!
}


/**
 * Main wrapper for function execution, preps input to be executed
 * Inputs:
 *   line: The command(s) to be executed
 **/
void execute(char * line) {
  if (line) // Remove trailing newline, and ignore null cmds
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

  while (cmds[i++])
    execute_single(cmds[i]);
}

/**
 *  Prompts the user and stores their input in buf
 * Inputs:
 *   buf: The buffer for the command to be stored in
 **/
void prompt_user(char * buf) {
  char wd[100];
  printf("\e[36mblue-shL:\e[33m%s\e[37m$ ", getcwd(wd, 100));
  fgets(buf, 100, stdin);
}


// Main
int main(){
  char *s = (char *)malloc(101);
  while ( 1 ){
    prompt_user(s);
    execute(s);
  }
  free(s);
  return 0;
}
