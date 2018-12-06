#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>
#include <signal.h>

using namespace std;

#define MAXLENGHT 1000
#define MAXLIST 100
#define clear() printf("\033[H\033[J")


void sigChld(int arg) {
    int pid, status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status)) {
        cout << endl << "background process " << pid << " done";
    }
    else {
        cerr << endl << "background process " << pid << " failed";
    }
}


bool parse_space(char* str, char** parsed, bool *background)
{
    char *last_symbol = "";
    int i = 0;

    for (i = 0; i < MAXLIST; i++)
    {
        parsed[i] = strsep(&str, " ");

        if (parsed[i] == NULL)
            break;

        if (strlen(parsed[i]) == 0)
            i--;
        else
            last_symbol = parsed[i];
    }

    if (last_symbol != "" && last_symbol[0] == '&') {
        parsed[i-1] = NULL;
        *background = true;
    }

//    for(i=0; i < MAXLIST; i++)
//    {
//        if (parsed[i][0] == '&') {
//            parsed[i - 1] = NULL;
//            *background = true;
//
//            break;
//        }
//    }
}


bool execute_cmd(char** parsed)
{
    pid_t pid;

    switch (pid = fork())
    {
        case -1:
            cerr << "Error in fork()";

            return false;
        case 0:
            if (execvp(parsed[0], parsed) < 0)
                cerr << "Could not execute command";

            return false;
        default:
            int status;
            if (waitpid(pid, &status, 0) > 0) {
                if (WIFEXITED(status))
                {
                    return true;
                }
                else{
                    cerr << "Child process terminated with error";

                    return false;
                }
            }
            else {
                cerr << "waitpid failed";

                return false;
            }
    }
}


bool execute_pipe(vector<char*> pipe_commands){
    char* command1 = pipe_commands[0];
    char* command2 = pipe_commands[1];

    bool background;

    char *parsed1[MAXLIST];
    parse_space(command1, parsed1, &background);

    char *parsed2[MAXLIST];
    parse_space(command2, parsed2, &background);

    int pfd[2];
    pipe(pfd);

    int pid1, pid2;

    switch (pid1 = fork()){
        case -1:
            cerr << "Error in fork()";

            return false;

        case 0:
            close(STDOUT_FILENO);
            dup2(pfd[1], STDOUT_FILENO);

            close(pfd[0]);
            close(pfd[1]);

            execvp(parsed1[0], parsed1);

        default:
            close(pfd[1]);

            wait(NULL);
    }

    switch (pid2 = fork()){
        case -1:
            cerr << "Error in fork()";

            return false;

        case 0:
            close(STDIN_FILENO);
            dup2(pfd[0], STDIN_FILENO);

            close(pfd[0]);
            close(pfd[1]);

            execvp(parsed2[0], parsed2);

        default:
            close(pfd[0]);

            wait(NULL);
    }

    return true;
}


bool execute_background(char** parsed){
    pid_t pid;

    switch (pid = fork())
    {
        case -1:
            cerr << "Error in fork()";

            return false;
        case 0:
            if (execvp(parsed[0], parsed) < 0)
                cerr << "Could not execute command";

            return false;
        default:
            signal(SIGCHLD, sigChld);
    }
}


bool read_command_cpp(string *str_cmd){
    cout << endl << "Your command: ";
    getline(cin, *str_cmd);

    return ((*str_cmd).length() > 0);
}


void split_by_delimiter(vector<char*> *commands, string str_command, string delim) {

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
            (*commands).push_back(ccommand);

            new_delimiter_position += delim.size();
            last_delimiter_position = new_delimiter_position;
        }
        else {
            string command = str_command.substr(last_delimiter_position);

            char * ccommand = new char[command.size()+1];
            copy(command.begin(), command.end(), ccommand);
            ccommand[command.size()] = '\0';
            (*commands).push_back(ccommand);

            delimiter_not_founded = true;
        }
    }
}


string define_delimeter(string command){
    if (command.find("&&") != -1){
        return "&&";
    }
    else{
        if (command.find("||") != -1){
            return "||";
        }
        else{
            if (command.find(",") != -1){
                return ",";
            }
            else {
                if (command.find("|") != -1) {
                    return "|";
                }
                else {
                    return "";
                }
            }
        }
    }
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

                string delimeter = define_delimeter(str_command);

                vector<char*> commands;

                if (delimeter != "") {
                    split_by_delimiter(&commands, str_command, delimeter);
                }
                else{
                    char * ccommand = new char[str_command.size()+1];
                    copy(str_command.begin(), str_command.end(), ccommand);
                    ccommand[str_command.size()] = '\0';

                    commands.push_back(ccommand);
                }

                if (delimeter != "|") {
                    for (unsigned long i = 0; i < commands.size(); i++) {

                        //background = str_command.size() > 1 && str_command[str_command.size()-1] == '&' && str_command[str_command.size()-2] != '&';

                        char *parsed[MAXLIST];

                        bool background;
                        parse_space(commands.at(i), parsed, &background);

                        if (!background) {
                            bool result;
                            result = execute_cmd(parsed);

                            if ((delimeter == "&&" && !result) || (delimeter == "||" && result)) {
                                break;
                            }
                        }
                        else{
                            bool result;
                            result = execute_background(parsed);
                        }
                    }
                }
                else {
                    execute_pipe(commands);
                }
            }
            else
                exit = true;
        }
    }

    return 0;
}
