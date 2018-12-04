#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>

using namespace std;

#define MAXLENGHT 1000
#define MAXLIST 100
#define clear() printf("\033[H\033[J")


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

            if (execvp(parsed[0], parsed) < 0)
                printf("\nCould not execute command..");

            exit(0);
        default:
            wait(NULL);

            return;
    }

}


bool read_command_cpp(string *str_cmd){
    cout << endl << "Your command: ";
    getline(cin, *str_cmd);

    return ((*str_cmd).length() > 0);
}


vector<char*> split_by_delimiter(string str_command, string delim) {
    vector<char*> commands;

    int last_delimiter_position = 0;
    int new_delimiter_position = 0;
    bool delimiter_not_founded = false;

    while(!delimiter_not_founded){
        new_delimiter_position = str_command.find(delim, new_delimiter_position);

        if (new_delimiter_position != -1) {
            string command = str_command.substr(last_delimiter_position, new_delimiter_position - last_delimiter_position);

            char * ccommand = new char[command.size()+1];
            copy(command.begin(), command.end(), ccommand);
            ccommand[command.size()] = '\0';
            commands.push_back(ccommand);

            new_delimiter_position++;
            last_delimiter_position = new_delimiter_position;
        }
        else {
            string command = str_command.substr(last_delimiter_position);

            char * ccommand = new char[command.size()+1];
            copy(command.begin(), command.end(), ccommand);
            ccommand[command.size()] = '\0';
            commands.push_back(ccommand);

            delimiter_not_founded = true;
        }
    }

    return commands;
}

vector<string> commands_storage;
int cur_command = 0;
int main()
{
    string str_command;

    bool exit = false;

    while(not exit) {
        if (read_command_cpp(&str_command)) {
            if (str_command != "exit()") {

                commands_storage.push_back(str_command);

                vector<char *> commands = split_by_delimiter(str_command, "|");

                for (unsigned long i = 0; i < commands.size(); i++) {
                    char *parsed[MAXLIST];
                    parse_space(commands.at(i), parsed);

                    execute_cmd(parsed);
                }
            }
            else
                exit = true;
        }
    }

    return 0;
}
