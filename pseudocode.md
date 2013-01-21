```
main():
    create output pipe
    create translate pipe
    fork output()
    fork translate()
    execute input() as parent

input(outputpipe, translatepipe):
    while (1):
        get user input from stdin

        buffer user input

        if user input is 'E'
            write the buffer to translatepipe
            clear buffer
        else if user input is 'T'
            terminate ##
        else if user input is 'ctrl-k'
            terminate abnormally ##
        else
            write to outputpipe

output(pipe):
    start thread for reading pipe from translate

    while (1):
        read from pipe one character at a time
        output the character to stdout

translate(pipe):
    initialize translatedline

    while(1):
        read from pipe one character at a time
        if character is 'K':
            clear translatedline
        else if character is 'X':
            erase one character at end of translatedline
        else if character is 'a':
            append 'z' to translatedline
        else if character is 'E':
            send translatedline to translate_output pipe
            clear translatedline
        else
            append character to translatedline
```
