main():
    create output pipe
    create translate pipe
    fork output
    fork translate
    execute input() as parent

input():
    get user input from stdin
    write to translate pipe
    write to output pipe

output():
    read from input pipe
    read from translate pipe
    display to stdout

translate():
    read from input pipe
    if input is 'ctrl-k'
        abnormal termination #elaborate more on these results
    if input is 'K'
        delete entire line
    if input is 'X'

