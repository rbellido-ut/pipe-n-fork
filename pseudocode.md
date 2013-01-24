```
main():
    disable terminal
    create output pipe
    create translate pipe
    create translate_output pipe
    fork output()
    fork translate()
    execute input() as parent
    terminate processes
    enable terminal

input(input_outputpipe, input_translatepipe):
    while (1):
        get user input from stdin

        buffer user input

        if user input is 'E':
            write the buffer to input_translatepipe
            clear buffer
        else if user input is 'T':
            write user input to input_outputpipe
            write buffered user input to input_translatepipe
            return
        else if user input is 'ctrl-k':
            return
        else:
            write user input to input_outputpipe

output(input_outputpipe, translate_outputpipe):
    start thread for reading pipe from translate_outputpipe

    while (1):
        read from input_outputpipe one character at a time
        output the character to stdout

        if user input is 'T':
            wait for translate to finish
            terminate

readfromtranslate(translate_outputpipe):
    while (1):
        read from translate_outputpipe one character at a time
        output character to stdout

        if user input is 'T':
            flush the output
            exit out of loop

translate(input_translatepipe, translate_outputpipe):
    initialize translatedline

    while(1):
        read from input_translatepipe one character at a time
        if character is 'K':
            clear translatedline
        else if character is 'X':
            erase one character at end of translatedline
        else if character is 'a':
            append 'z' to translatedline
        else if character is 'T':
            write translatedline to translate_outpipe
            return
        else if character is 'E':
            send translatedline to translate_outputpipe
            clear translatedline
        else:
            append character to translatedline
```
