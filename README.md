# Fall 2016 GMU Google Programming Contest Solutions

This repo contains my solutions to the Fall 2016 Google Programming Contest at GMU. The problems themselves can be viewed [here](https://sites.google.com/site/fall2016gmuprogrammingcontest/home).

## Building

Each problem has a seperate folder for it's solutions, and may have zero or more implementations in various languages.

All solutions are to be built and run from within their own directories; such as:

```
tgrehawi@skaia:~/ggpc-fall16/problem1 $ make
tgrehawi@skaia:~/ggpc-fall16/problem1 $ problem1 < /path/to/input
...
tgrehawi@skaia:~/ggpc-fall16/problem1 $ make clean

```

### C
Build with `$ make`, then run with `$ problem#` where # is replaced with the problem number (1, 2, or 3).

Clean the build with `$ make clean`.

### Python
Python solutions can be run just like any command: `$ ./problem#.py < /path/to/input`, or the traditional way with `$ python problem#.py`.

### Lua
Lua solutions can be run just like Python ones, but with `$ lua` instead of `$ python`. They are also executable directly from the command line.
