#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <pthread.h>
#include <stdio.h>

// Main functions //
void input(int outfd[2], int in_transfd[2]);
void output(int outfd[2], int trans_outfd[2]);
void translate(int in_transfd[2], int trans_outfd[2]);

// Utility functions //
void terminate(pid_t childpid);
void fatal(std::string err_msg);
void* readfromtranslate(void *translatepipe);
void term_sig_handler();

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
        terminate(transpid);
    }
    else if ((outpid = fork()) == 0) //output code (child)
    {
        output(outputfd, translate_outfd);
        terminate(outpid);
    }
    else //input code (parent)
    {
        input(outputfd, in_translatefd);
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
            /*if (write(outfd[1], (char*) &in, 1) == 0)
                fatal("error in input(). write() failed\n");*/

            if (write(in_transfd[1], (char*) buffer.c_str(), buffer.size()) == 0)
                fatal("error in input(). write() failed\r\n");

            wait((int*) 0);
            wait((int*) 0);

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
            //cout << "scanned T...output" << endl;
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
       if (read((int) translatepipe, &transout, 1) == 0)
           fatal("read() from readfromtranslate() failed\r\n");

       cout << transout;
       cout.flush();

       if (transout == 'T')
       {
           //cout << "scanned T...readfromtranslate" << endl;
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
