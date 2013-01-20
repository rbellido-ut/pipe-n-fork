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
    while (1):
        read from pipe one character at a time
        output the character to stdout

translate(pipe):
    while(1):
        read from pipe one character at a time


translateline(line):
    for each character in line:
        if character is 'K'
            return empty translated line
        if character is 'X'
            don't add character to translated line
        if character is 'a'
            replace character by 'z' in translated line

    return translated line
```
