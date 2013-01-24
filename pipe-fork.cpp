#include <iostream>
#include <unistd.h>
#include "pipe-fork.h"

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
        wait((int*) 0);
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
            fatal("read() failed in translate()\r\n");

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

            case 'E': //fall-through

            case 'T': //end of characters, send translated line to output()
                translatedline += "\r\n";

                signal(SIGTERM, term_sig_handler);

                if (write(trans_outfd[1], (char *) translatedline.c_str(), translatedline.size()) == 0)
                    fatal("write() failed in translate()\r\n");

                translatedline = "\r\n";

                if (recvdchar == 'E')
                    break;

                return;

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
                fatal("error in input(). write() failed\r\n");

            buffer = "";
        }
        else if (in == 'T')
        {
            //send buffered message to be translated
            if (write(in_transfd[1], (char*) buffer.c_str(), buffer.size()) == 0)
                fatal("error in input(). write() failed\r\n");
            break;
        }
        else if (in == 11) //terminate abnormally
        {
            break;
        }
        else
        {
            if (write(outfd[1], (char *) &in, 1) == 0)
                fatal("error in input(). write() failed\r\n");
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
            fatal("read from output pipe failed\r\n");

        cout << inout;
        cout.flush();

        if (inout == 'T')
        {
            cout.flush();
            break;
        }

    }
}

void* readfromtranslate(void *translatepipe)
{
   char transout;

   while (1)
   {
       if (read((int) translatepipe, &transout, sizeof(char)) == 0)
           fatal("read() from readfromtranslate() failed\r\n");

       cout << transout;
       cout.flush();

       if (transout == 'T')
       {
           cout.flush();
           break;
       }
   }

   return NULL;
}


void terminate(pid_t childpid)
{
    if (kill(childpid, SIGTERM) == -1)
        fatal("error killing child process");
}

void fatal(string err_msg)
{
    cerr << err_msg;
    system("stty -raw -igncr echo");
    exit(1);
}

static void term_sig_handler(int signo)
{
    fprintf(stdout, "%d", signo); //this doesn't actually get displayed..I don't even understand
}
