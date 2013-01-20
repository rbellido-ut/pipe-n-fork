#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string>
#include <fcntl.h>


// Main functions //
void input(int outfd[2], int in_transfd[2]);
void output(int outfd[2]);
void translate(int trans_outfd[2], int in_transfd[2]);

// Utility functions //
void terminate();
void fatal(std::string err_msg);
std::string translateline(std::string line);

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
        translate(translate_outfd, in_translatefd);
    }
    else if ((outpid = fork()) == 0) //output code (child)
    {
        output(outputfd);
    }
    else //input code (parent)
    {
        input(outputfd, in_translatefd);
    }

    cout << "exiting...";
    system("stty -raw -igncr echo");

    return 0;
}

void translate(int trans_outfd[2], int in_transfd[2])
{
    string buff, transline;
    char recvdchar;

    while (1)
    {
        if (read(in_transfd[0], (char *) &recvdchar, 1) == 0)
            fatal("read() failed in translate()\n");

        buff += recvdchar;

        transline = translateline(buff);

        if (write(trans_outfd[1], (char *) transline.c_str(), transline.size()) == 0)
            fatal("write() failed in translate()\n");
    }

}

std::string translateline(string line)
{
    string translatedline;
    string::size_type i = 0;

    for (i = 0; i < line.size(); ++i)
    {
        switch (line[i])
        {
            case 'K':
                return "\n";

            case 'X':
                break;

            case 'a':
                translatedline[i] = 'z';
                break;

            default:
                translatedline[i] = line[i];
                break;
        }
    }

    return translatedline;
}

void input(int outfd[2], int in_transfd[2])
{
    char in;
    string buffer;

    close(outfd[0]); //close read pipe
    close(in_transfd[0]);

    while (1)
    {
        cin >> in;


        if (in == 'E')
        {
            cout << buffer << endl;
            if (write(in_transfd[1], (char *) buffer.c_str(), buffer.size()) == 0)
                fatal("error in input(). write() failed\n");

            buffer = "";
        }
        else
        {
            buffer += in;

            if (write(outfd[1], (char *) &in, 1) == 0)
                fatal("error in input(). write() failed\n");
        }
    }
}

void output(int outfd[2])
{
    char inout;

    while (1)
    {
        if (read(outfd[0], &inout, 1) != 1)
            fatal("read from output pipe failed\n");

        cout << inout;
        cout.flush();
    }
}

void fatal(string err_msg)
{
    cerr << err_msg;
    exit(1);
}
