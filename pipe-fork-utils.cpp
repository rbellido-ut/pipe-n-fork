/*---------------------------------------------------------------------------------------------
-- SOURCE FILE: pipe-fork-utils.cpp
-- Implementation of the utility functions used by the pipe-n-fork program
--
-- PROGRAM: pipe-n-fork-utils
--
-- FUNCTIONS:
-- void terminate(pid_t)
-- void fatal(string)
-- void* readfromtranslate(void*)
-- void term_sig_handler(int)
--
-- DATE: January 22, 2013
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ronald Bellido
--
-- PROGRAMMER: Ronald Bellido
--
--------------------------------------------------------------------------------------------------*/
#include "pipe-fork.h"

using namespace std;


/*-------------------------------------------------------------------------------------------------
-- FUNCTION: readfromtranslate
--
-- DATE: January 21, 2013
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ronald Bellido
--
-- PROGRAMMER: Ronald Bellido
--
-- INTERFACE: void* readfromtranslate(void *translatepipe)
--          translatepipe - the read descriptor from the translate-output pipe
--
-- RETURNS: void*
--
-- NOTES:
Function that is executed in a thread created by output() in pipe-fork.cpp.
It read character by character from the file descriptor provided and displays it to stdout.
-------------------------------------------------------------------------------------------------*/
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


/*-------------------------------------------------------------------------------------------------
-- FUNCTION: fatal
--
-- DATE: January 16, 2013
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ronald Bellido
--
-- PROGRAMMER: Ronald Bellido
--
-- INTERFACE: void fatal(string err_msg)
--          err_msg - the error message to display
--
-- RETURNS: void
--
-- NOTES:
Displays the string passed to standard error and ensures that the terminal is back to
normal before closing the program.
-------------------------------------------------------------------------------------------------*/
void fatal(string err_msg)
{
    cerr << err_msg;
    system("stty -raw -igncr echo");
    exit(1);
}


/*-------------------------------------------------------------------------------------------------
-- FUNCTION: terminate
--
-- DATE: January 21, 2013
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ronald Bellido
--
-- PROGRAMMER: Ronald Bellido
--
-- INTERFACE: void terminate(pid_t chilpid)
--          childpid - the pid of the (child) process to kill
--
-- RETURNS: void
--
-- NOTES:
This function is a wrapper for the kill() function.
-------------------------------------------------------------------------------------------------*/
void terminate(pid_t childpid)
{
    if ((kill(childpid, SIGTERM)) == -1)
        fatal("error killing child process!\r\n");
}


/*-------------------------------------------------------------------------------------------------
-- FUNCTION: term_sig_handler
--
-- DATE: January 23, 2013
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ronald Bellido
--
-- PROGRAMMER: Ronald Bellido
--
-- INTERFACE: void term_sig_handler(int signo)
--          signo - the signal number
--
-- RETURNS: void
--
-- NOTES:
This is the handler for the signal() call that's made when the program is terminating normally
and needs to let other processes finish.
-------------------------------------------------------------------------------------------------*/
void term_sig_handler(int signo)
{
    fprintf(stdout, "%d", signo); //this doesn't actually get displayed..I don't even understand why
}
