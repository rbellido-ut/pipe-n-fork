#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>

void input(int outfd[2], int transfd[2]);
void output(int outfd[2]);
void translate();
void fatal(char *);

int main(void)
{
    //TODO: Disable UNIX commands
    pid_t transpid, outpid;
    int translatefd[2], outputfd[2];

    if (pipe(translatefd) < 0)
        fatal("error in creating pipe");
    if (pipe(outputfd) < 0)
        fatal("error in creating pipe");

    if ((transpid = fork()) == 0) //translate code (child)
    {
        translate(translatefd);
    }
    else if ((outpid = fork()) == 0) //output code (child)
    {
        output(outputfd);
    }
    else //input code (parent)
    {
        input(outputfd, translatefd);
    }

    //TODO: re-enable UNIX commands before exiting

    return 0;
}

void translate(int transfd[2])
{
    close(transfd[1]);
}

void input(int outfd[2], int transfd[2])
{
    int in;

    close(outfd[0]);
    close(transfd[0]);

    while (1)
    {
        if ((in = fgetc(stdin)) == EOF)
            fatal("error in fgetc");

        switch (in)
        {
            case 'T':
                fprintf(stdout, "terminate");
                break;

            //case ctrl-k

            case 'E':
                break;

            default: //write to output pipe
                if (write(outfd[1], (int*) &in, sizeof(int)) == 0)
                    fatal("error writing to output pipe");
                break;
        }
    }
}

void output(int outfd[2])
{
    int out;

    close(outfd[1]);

    while (1)
    {
        read(outfd[0], &out, 1);
        fputc(out, stdout);
    }
}

void fatal(char *err_msg)
{
    perror(err_msg);
    exit(1);
}
