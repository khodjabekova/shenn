#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
using namespace std;

#define MAXLENGHT 1000
#define MAXLIST 100

#define clear() printf("\033[H\033[J")

void start()
{
    clear();
    printf("Welcome!\n");
}

void get_current_dir()
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("\nDir: %s \n", cwd);
}

void help()
{
    puts("еще не придумали, что сюда написать:"
          "\n>cd"
          "\n>ls"
          "\n>exit");

    return;
}

bool read_command(char str[MAXLENGHT])
{
    string cmd;
    cin >> cmd;

    if (cmd.length() > 0)
    {
        cmd.copy(str, cmd.length(), 0);
        return true;
    }
    else
    {
        return false;
    }
}

bool parse_pipe(char* str, char** strpiped)
{
    int i;
    for (int i = 0; i < 2; i++)
    {
        strpiped[i] = strsep(&str, "|");
        if (strpiped[i] == NULL)
            break;
    }

    if(strpiped[1] == NULL)
      return false;
    else
      return true;
}
bool parse_space(char* str, char** parsed)
{
    for (int i = 0; i < MAXLIST; i++)
    {
        parsed[i] = strsep(&str, " ");

        if (parsed[i] == NULL)
            break;
        if (strlen(parsed[i]) == 0)
            i--;
    }
}
void execute_cmd(char** parsed)
{
    pid_t pid;

    switch (pid = fork())
    {
      case -1:
        printf("\n при вызове fork() возникла ошибка");
        return;
      case 0:
        printf(" CHILD: Это процесс-потомок!\n");
        if (execvp(parsed[0], parsed) < 0)
              printf("\nCould not execute command..");

          exit(0);
      default:
        wait(NULL);
        return;
    }

}

int main()
{
    char command[MAXLENGHT];
    char* parsed[MAXLIST];
    char* parsed_pipe[MAXLIST];
    char str[MAXLENGHT];

    //start();
    //get_current_dir();
    //help();

      if(read_command(str)){
        char* str_piped[2];
        bool piped = false;
        piped = parse_pipe(str, str_piped);

        if (piped)
        {
            parse_space(str_piped[0], parsed);
            parse_space(str_piped[1], parsed_pipe);
        }
        else
        {
            parse_space(str, parsed);
        }

        execute_cmd(parsed);
      }


    return 0;
}
