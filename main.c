#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

// Main function //
void input(int outfd[2], int transfd[2]);
void output(int outfd[2]);
void translate(int outfd[2], int transfd[2]);
void fatal(char *);

// Utility functions //
void killline(char *line);
void backspace(char *line);

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
        //translate(outputfd, translatefd);
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

/*void translate(int outfd[2], int transfd[2])
{
    size_t i = 0;
    int in = 0;
    int *buf = 0, **p;

    close(transfd[1]); //close the write descriptor on translate pipe
    close(outfd[0]); //close read descriptor on output pipe

    while (1)
    {
        read(transfd[0], &in, 1);

        switch (in)
        {
            case 'a':
                in = 'z';
                strncat(buf, (char*) in, 1);
                break;

            case 'E':
                for (p = buf[0]; *p != '\0';p++)
                {
                    if (write(outfd[1], (int*) buf[i], 1))
                        fatal("error writing buf to output pipe");
                }

                break;
            default:
                strncat(buf, (char*) &in, 1);
                break;
        }
    }
}*/

void input(int outfd[2], int transfd[2])
{
    char in = 0;

    close(outfd[0]);
    close(transfd[0]);

    while (1)
    {
        if (fscanf(stdin, "%c", &in) == EOF)
            fatal("error in fscanf\n");

        switch (in)
        {
            case 'T':
                break;

            case 11: //ctrl-k
                break;

            case 'E':
                if (write(transfd[1], (char*) &in, 1) == 0)
                    fatal("error writing to translate pipe\n");
                break;

            default: //write to output pipe
                if (write(outfd[1], (char*) &in, 1) == 0)
                    fatal("error writing to output pipe\n");
                break;
        }
    }
}

void output(int outfd[2])
{
    char out = 0;

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
