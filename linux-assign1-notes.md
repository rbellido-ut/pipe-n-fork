Title: Linux assignment 1 Notes
Date 2013-10-1

Do this incrementally
-----------------------

* Get 3 processes going first.
* incrementally add to the program.

Design Notes
--------------

* no zombies! kill children!
* screenshots for test docs/user doc
* might need `kill -9 pid`
* To kill all terminal commands:
    * `system("stty raw igncr -echo");` call this in the beginning
    * `system("stty -raw -igncr echo");` call this at the end before exiting program
    * If I never get to the end (e.g. crash before gets to end code) type in terminal:
    `stty sane ^j`

Tests
------

    abcdefE
    zbcdef

    abcdefXXE
    zbcd

    abcdefK(E)
    -empty-

    abcdefT(E)
    zbcdef
    #program terminates
