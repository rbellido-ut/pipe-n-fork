```
main():

    create output pipe
    create translate pipe
    fork output
    fork translate
    execute input() as parent

input():

    get user input from stdin

    if input is 'T'
        terminate program
    if input is 'ctrl-k'
        abnormal termination
    if input is 'E'
        write to translate pipe
    else
        write to output pipe

output():

    read from input pipe
    read from translate pipe
    display to stdout

translate():

    read from input pipe

    if input is 'K'
        delete entire line ##
    if input is 'X'
        delete one character back ##
    if input is 'a'
        write 'z' to output pipe
```
