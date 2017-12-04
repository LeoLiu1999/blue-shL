# blue-shL
(The Bad, Large, Unnecessarily extra sheL)
#### by Team Taking The shL: Leo Liu & Jake Zaia

## Features
* Can execute all (non builtin) BASH commands!
* Can parse multiple commands on one line!
* Can redirect using > & < (even if multiple are used)!
* Is blue!

## Bugs:
* Putting more than one space between tokens sometimes breaks things
* To exit C-d or the "exit" command sometimes have to be ran thrice
* ALso is C-d is used, some malloc'd memory is not free'd
* The * and & characters are useless and do not work as in bash


## Function Headers:

### void redirect_stdin( char **chargs, int i )
* Changes STDIN_FILENO to target
* Inputs:
    * chargs: All of the commands to be executed
    * i: The index of the character '<' in chargs

### void redirect_stdout( char **chargs, int i )
* Changes STDOUT_FILENO to target
* Inputs:
    * chargs: All of the commands to be executed
    * i: The index of the character '>' in chargs

### char **parse_args(char *line, char *delimiter)
 * Parses "line" as a command, separating on delimiter
 * Inputs:
    *   line: A line of user input
    *   delimiter: The character to use for strsep
 * Outputs:
    *   An array of strings representing tokens of user input

### void special_funcs(char ** chargs)
 * Handles shell builtins (cd & exit)
    *   chargs: All the commands to be executed

### void execute_single(char * line)
 * Executes a single bash command
 * Inputs:
    *   line: The command to be executed

### void execute(char * line)
 * Main wrapper for function execution, preps input to be executed
 * Inputs:
     *   line: The command(s) to be executed

### void prompt_user(char * buf)
 *  Prompts the user and stores their input in buf
 * Inputs:
     *   buf: The buffer for the command to be stored in
