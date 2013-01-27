Pipe-n-fork
============
A simple Linux program that uses IPC pipes and forks to interact with the user

This is my first assignment for my fourth term data communications option.

The assignment was to develop a simple command line program that interacts with the user.
When executed, it will disable all UNIX commands. Everytime the user presses a key,
that key's character is sent over a pipe that will buffer it. On command, the program will
display a translation of the buffered character.
