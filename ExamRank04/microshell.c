#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int ft_strlen(char *str)
{
    int i = 0;

    while (str[i])
        i++;
    return (i);
}

void ft_error(char *err_message, char *err_arg)
{
    write(2, err_message, ft_strlen(err_message));
    write(2, err_arg, ft_strlen(err_arg));
    write(2, "\n", 1);
    exit(1);
}

void ft_cd(char **command)
{
    int i = 1;

    while(command[i])
        i++;
    if (i != 2)
        ft_error("error: cd: bad arguments", "");
    if (chdir (command[1]))
        ft_error("error: cd: cannot change directory to ", command[0]);
}

int main(int argc, char **argv, char **env)
{
    int i = 1;
    int pipe_after_command;
    int pipes[2];
    int pid;
    char **command;
    int status;

    while (i < argc)
    {
        pipe_after_command = 0;
        command = &argv[i];
        while (i < argc && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
            i++;
        if (i < argc && !strcmp(argv[i], "|"))
            pipe_after_command = 1;
        argv[i] = 0;
        if (command[0])
        {
            if (!pipe_after_command && !strcmp(command[0], "cd"))
                ft_cd(command);
            if (pipe_after_command)
                pipe(pipes);
            pid = fork();
            if (pid < 0)
                ft_error("error: fatal\n", "");
            if (pid == 0)
            {
                if (pipe_after_command)
                    dup2(pipes[1], 1);
                if (!strcmp(command[0], "cd"))
                    ft_cd(command);
                else if (execve(command[0], command, env))
                    ft_error("error: cannot execute ", command[0]);
                exit(0);
            }
            else waitpid(pid, &status, 0);
            if (pipe_after_command)
            {
                dup2(pipes[0], 0);
                close(pipes[0]);
                close(pipes[1]);
            }
        }
        i ++;
    }
    return 0;
}