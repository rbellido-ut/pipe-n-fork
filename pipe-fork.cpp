#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>

// Main functions //
void input(int outfd[2], int in_transfd[2]);
void output(int outfd[2], int trans_outfd[2]);
void translate(int in_transfd[2], int trans_outfd[2]);

// Utility functions //
void terminate(pid_t tpid, pid_t outpid);
void fatal(std::string err_msg);
void* readfromtranslate(void *translatepipe);

using namespace std;

int main(void)
{
    system("stty raw igncr -echo");

    pid_t transpid, outpid;
    int in_translatefd[2], outputfd[2], translate_outfd[2];

    if (pipe(in_translatefd) < 0)
        fatal("error in creating pipe");

    if (pipe(outputfd) < 0)
        fatal("error in creating pipe");

    if (pipe(translate_outfd) < 0)
        fatal("error in creating pipe");

    if ((transpid = fork()) == 0) //translate code (child)
    {
        translate(in_translatefd, translate_outfd);
    }
    else if ((outpid = fork()) == 0) //output code (child)
    {
        output(outputfd, translate_outfd);
    }
    else //input code (parent)
    {
        input(outputfd, in_translatefd);

        terminate(transpid, outpid); //won't reach this until input() returns
    }

    system("stty -raw -igncr echo");

    return 0;
}

void translate(int in_transfd[2], int trans_outfd[2])
{
    string translatedline = "\r\n";
    char recvdchar;

    while (1)
    {
        if (read(in_transfd[0], (char *) &recvdchar, 1) == 0)
            fatal("read() failed in translate()\n");

        switch (recvdchar)
        {
            case 'K':
                translatedline.clear();
                translatedline = "\r\n";
                break;

            case 'X':
                translatedline.erase(translatedline.end() - 1);
                break;

            case 'a':
                translatedline += 'z';
                break;

            case 'E': //end of characters, send translated line to output()
                translatedline += "\r\n";

                if (write(trans_outfd[1], (char *) translatedline.c_str(), translatedline.size()) == 0)
                    fatal("write() failed in translate()\n");

                translatedline = "\r\n";
                break;

            default:
                translatedline += recvdchar;
                break;
        }
    }
}

void input(int outfd[2], int in_transfd[2])
{
    char in;
    string buffer = "";

    while (1)
    {
        in = getchar();

        buffer += in;

        if (in == 'E')
        {
            if (write(in_transfd[1], (char *) buffer.c_str(), buffer.size()) == 0)
                fatal("error in input(). write() failed\n");

            buffer = "";
        }
        else if (in == 'T')
        {
            break;
        }
        else if (in == 11) //terminate abnormally
        {
            break;
        }
        else
        {
            if (write(outfd[1], (char *) &in, 1) == 0)
                fatal("error in input(). write() failed\n");
        }
    }
}

void output(int outfd[2], int trans_outfd[2])
{
    char inout;
    pthread_t translate_outthread;

    pthread_create(&translate_outthread,
            NULL,
            readfromtranslate,
            (void*) trans_outfd[0]);

    while (1)
    {
        if (read(outfd[0], &inout, 1) == 0)
            fatal("read from output pipe failed\n");

        cout << inout;
        cout.flush();
    }
}

void* readfromtranslate(void *translatepipe)
{
   char transout;

   while (1)
   {
       if (read((int) translatepipe, &transout, 1) == 0)
           fatal("read() from readfromtranslate() failed\n");

       cout << transout;
       cout.flush();
   }

   return NULL;
}


void terminate(pid_t transpid, pid_t outpid)
{
    if (kill(transpid, 9) == -1)
        fatal("error killing translate process");

    if (kill(outpid, 9) == -1)
        fatal("error killing output process");
}

void fatal(string err_msg)
{
    cerr << err_msg;
    exit(1);
}
