/*---------------------------------------------------------------------------------------------
-- SOURCE FILE: pipe-fork.cpp
-- A linux command line application that interacts with the user, but not in the way you think.
--
-- PROGRAM: pipe-n-fork
--
-- FUNCTIONS:
-- void input(int outfd[2], int in_transfd[2])
-- void output(int outfd[2], int trans_outfd[2])
-- void translate(int in_transfd[2], int trans_outfd[2])
--
-- DATE: January 16, 2013
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ronald Bellido
--
-- PROGRAMMER: Ronald Bellido
--
-- NOTES:
This program creates three processes and uses three pipes for passing user input (characters)
across processes. User input is passed on to a translator and the translated input
is passed to output.

The following is the translation of user input:
    Input -> Output/Result
    -----------
    'a' -> 'z'
    'K' -> clears the line
    'X' -> backspace
    'E' -> enter
    'T' -> exits the program, but not before outputting translated input
    ctrl-k -> abnormal termination, no translated input will be shown

This program uses the lpthread library.
--------------------------------------------------------------------------------------------------*/
#include "pipe-fork.h"

using namespace std;

/*--------------------------------------------------------------------------------------------------
-- FUNCTION: main
--
-- DATE: January 16, 2013
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ronald Bellido
--
-- PROGRAMMER: Ronald Bellido
--
-- INTERFACE: int main(void)
--
-- RETURNS: 0 - to exit
--
-- NOTES:
pipe-n-fork program's main entry point.
This is where three pipes are created. A pipe from input to translate, from translate to output,
and lastly from input to output. This function is also where two separate child processes
are created by fanning. The parent child handles the user input, and the two child
proceses are the translate and output functions.
-------------------------------------------------------------------------------------------------*/
int main(void)
{
    system("stty raw igncr -echo");

    pid_t transpid, outpid;
    int in_translatefd[2], outputfd[2], translate_outfd[2];
    int input_ret;

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
        if ((input_ret = input(outputfd, in_translatefd)) == 0)
        {
            //terminate normally
            wait((int*) 0);
        }
        else if (input_ret == 1)
        {
            terminate(outpid);
            terminate(transpid);
        }
    }

    system("stty -raw -igncr echo");

    return 0;
}


/*-------------------------------------------------------------------------------------------------
-- FUNCTION: translate
--
-- DATE: January 21, 2013
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ronald Bellido
--
-- PROGRAMMER: Ronald Bellido
--
-- INTERFACE: void translate(int in_transfd[2], int trans_outfd[2])
--          in_transfd - the file descriptors for the input-translate pipe
--          trans_outfd - the file descriptors for the translate-output pipe
--
-- RETURNS: void
--
-- NOTES:
This function is passed two file descriptors. It reads
charater by character from the input-translate pipe, then it buffers the characters
into a string after processing/translating it. Finally, it writes the entire translated
string to the translate-output pipe.
-------------------------------------------------------------------------------------------------*/
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

                signal(SIGTERM, term_sig_handler);
                return;

            default:
                translatedline += recvdchar;
                break;
        }
    }
}


/*-------------------------------------------------------------------------------------------------
-- FUNCTION: input
--
-- DATE: January 16, 2013
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ronald Bellido
--
-- PROGRAMMER: Ronald Bellido
--
-- INTERFACE: void input(int outfd[2], int in_transfd[2])
--          in_transfd - the file descriptors for the input-translate pipe
--          outfd - the file descriptors for the input-output pipe
--
-- RETURNS: 0 - if terminating normally (user presses 'T')
--          1 - if terminating abnormally (user presses 'ctrl-k')
--
-- NOTES:
This function takes two file descriptors and writes to each one: input-output pipe
and input-translate pipe. It also performs some of the program command
processing such as 'T', ctrl-k, and 'E'.

Initial user input is buffered and writes the characters typed to the
input-output pipe. When the user presses 'E', the entire buffered string is sent to the
translate-output pipe.
-------------------------------------------------------------------------------------------------*/
int input(int outfd[2], int in_transfd[2])
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
            if (write(outfd[1], (char*) &in, 1) == 0)
                fatal("error in input(). write() failed\r\n");

            //send buffered message to be translated
            if (write(in_transfd[1], (char*) buffer.c_str(), buffer.size()) == 0)
                fatal("error in input(). write() failed\r\n");

            return 0;
        }
        else if (in == 11) //terminate abnormally
        {
            return 1;
        }
        else
        {
            if (write(outfd[1], (char *) &in, 1) == 0)
                fatal("error in input(). write() failed\r\n");
        }
    }
}

/*-------------------------------------------------------------------------------------------------
-- FUNCTION: output
--
-- DATE: January 16, 2013
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ronald Bellido
--
-- PROGRAMMER: Ronald Bellido
--
-- INTERFACE: void input(int outfd[2], int trans_outfd[2])
--          trans_outfd - the file descriptors for the input-translate pipe
--          outfd - the file descriptors for the input-output pipe
--
-- RETURNS: void
--
-- NOTES:
This function reads character by character from both file descriptors. The outfd file descriptor
is used to read characters passed in the input-output pipe. The trans_outfd file descriptor
is used on a separate thread created by this function. While this function's sole job is to
output characters as they get read from the pipe, it also processes 'T' to signal a termination
of the calling process.
-------------------------------------------------------------------------------------------------*/
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
            signal(SIGTERM, term_sig_handler);
            cout.flush();
            break;
        }
    }
}
