#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

char *lime = "\x1b[38;5;10m";
char *yellow = "\x1b[38;5;11m";
char *blue = "\x1b[38;5;12m";
char *white = "\x1b[38;5;15m";

void get_cmd_args(char *linea, char **args);
void cmd_with_pipes(char *cmd1, char *cmd2, char *cmd3);
void cmd_propio(char **args);
void redireccionamiento(char **args);

int main()
{
    char linea[BUFFER_SIZE];
    char *args[10];
    char *username = getenv("USER");

    while (1)
    {
        printf("%s%s%s@soshell> %s", yellow, username, lime, white);
        fflush(stdout);
        if (!fgets(linea, sizeof(linea), stdin))
            break;

        char *cmd1 = strtok(linea, "|");
        char *cmd2 = strtok(NULL, "|");
        char *cmd3 = strtok(NULL, "|");

        if (cmd3)
            cmd_with_pipe(cmd1, cmd2, cmd3);

        else if (cmd2)
            cmd_with_pipe(cmd1, cmd2, NULL);

        else
        {
            get_cmd_args(cmd1, args);

            if (args[0] == NULL)
                continue;

            if (strcmp(args[0], "exit") == 0)
                break;

            if (strcmp(args[0], "cd") == 0)
            {
                if (args[1] == NULL)
                    chdir(getenv("HOME"));
                else
                {
                    if (chdir(args[1]) != 0)
                        perror("Error al cambiar de directorio.\n");
                }
                continue;
            }

            if ((strcmp(args[0], "rot47") == 0) || (strcmp(args[0], "crear") == 0) || (strcmp(args[0], "eliminar") == 0))
                cmd_propio(args);

            else
            {
                if (fork() == 0)
                {
                    redireccionamiento(args);
                    execvp(args[0], args);
                    perror("Error al ejecutar el comando.\n");
                    exit(EXIT_FAILURE);
                }
                wait(NULL);
            }
        }
    }
    return 0;
}

void get_cmd_args(char *linea, char **args)
{
    char *token;
    token = strtok(linea, " \n");
    int counter = 0;

    while (token != NULL)
    {
        args[counter++] = token;
        token = strtok(NULL, " \n");
    }
    args[counter] = NULL;
}

void cmd_with_pipes(char *cmd1, char *cmd2, char *cmd3)
{
    int fd[2], fd2[2];
    char *args1[10], *args2[10], *args3[10];

    get_cmd_args(cmd1, args1);
    get_cmd_args(cmd2, args2);

    if (cmd3)
        get_cmd_args(cmd3, args3);

    pipe(fd);

    if (fork() == 0) // proceso hijo cmd 1
    {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);

        if ((strcmp(args[0], "rot47") == 0) || (strcmp(args[0], "crear") == 0) || (strcmp(args[0], "eliminar") == 0)) // cmd propio
            cmd_propio(args1);
        else
        {
            redireccionamiento(args1);
            execvp(args1[0], args1);
            perror("Error al ejecutar el comando.\n");
            exit(EXIT_FAILURE);
        }
    }
    if (cmd3)
    {
        pipe(fd2);
        if (fork() == 0) // proceso hijo cmd2
        {
            dup2(fd[0], STDIN_FILENO);
            dup2(fd2[1], STDOUT_FILENO);

            close(fd[0]);
            close(fd[1]);
            close(fd2[0]);
            close(fd2[1]);

            if ((strcmp(args[0], "rot47") == 0) || (strcmp(args[0], "crear") == 0) || (strcmp(args[0], "eliminar") == 0)) // cmd propio
                cmd_propio(args2);
            else
            {
                redireccionamiento(args2);
                execvp(args2[0], args2);
                perror("Error al ejecutar el comando.\n");
                exit(EXIT_FAILURE);
            }
        }

        if (fork() == 0) // proceso hijo cmd3
        {
            dup2(fd2[0], STDIN_FILENO);

            close(fd2[0]);
            close(fd2[1]);
            close(fd[0]);
            close(fd[1]);

            if ((strcmp(args[0], "rot47") == 0) || (strcmp(args[0], "crear") == 0) || (strcmp(args[0], "eliminar") == 0)) // cmd propio
                cmd_propio(args3);
            else
            {
                redireccionamiento(args3);
                execvp(args3[0], args3);
                perror("Error al ejecutar el comando.\n");
                exit(EXIT_FAILURE);
            }
        }
        close(fd2[0]);
        close(fd2[1]);
    }
    else
    {
        if (fork() == 0) // proceso hijo cmd2
        {
            dup2(fd[0], STDIN_FILENO);

            close(fd[0]);
            close(fd[1]);

            if ((strcmp(args[0], "rot47") == 0) || (strcmp(args[0], "crear") == 0) || (strcmp(args[0], "eliminar") == 0))
                cmd_propio(args2);

            else
            {
                redireccionamiento(args2);
                execvp(args2[0], args2);
                perror("Error al ejecutar el comando.\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    // cerramos el pipe en el padre
    close(fd[0]);
    close(fd[1]);

    wait(NULL); // esperamos a los hijos
    wait(NULL);

    if (cmd3)
        wait(NULL);
}

void cmd_propio(char **args)
{
    if (strcmp(args[0], "rot47") == 0)
    {
        if (fork() == 0)
        {
            execl("./rot47", "rot47", args[1], (char *)NULL);
            perror("Error al ejecutar el comando rot47.\n");
            exit(EXIT_FAILURE);
        }
        wait(NULL);
    }
    else if (strcmp(args[0], "crear") == 0)
    {
        if (fork() == 0)
        {
            execl("./crear", "crear", args[1], (char *)NULL);
            perror("Error al ejecutar el comando crear.\n");
            exit(EXIT_FAILURE);
        }
        wait(NULL);
    }
    else if (strcmp(args[0], "eliminar") == 0)
    {
        if (fork() == 0)
        {
            execl("./eliminar", "eliminar", args[1], (char *)NULL);
            perror("Error al ejecutar el comando eliminar.\n");
            exit(EXIT_FAILURE);
        }
        wait(NULL);
    }
    else
        printf("El comando %s no fue reconocido.\n", args[0]);
}

void redireccionamiento(char **args)
{
    for (int idx = 0; args[idx] != NULL; idx++)
    {
        if (strcmp(args[idx], ">") == 0)
        {
            int fd = open(args[idx + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0)
            {
                perror("Error al abrir el archivo");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            args[idx] = NULL;
            break;
        }
        else if (strcmp(args[idx], "<") == 0)
        {
            int fd = open(args[idx + 1], O_RDONLY);
            if (fd < 0)
            {
                perror("Error al abrir el archivo");
                exit(EXIT_FAILURE);
            }

            dup2(fd, STDIN_FILENO);
            close(fd);
            args[idx] = NULL;
            break;
        }
    }
}
