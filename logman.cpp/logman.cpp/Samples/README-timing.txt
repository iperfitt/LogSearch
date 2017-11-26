Most sample command files use a subset of the commands, look at each file to see which
commands it tests.

I ran timings on the autograder, but remember that your times may vary!  A slower computer,
using a debug build (-g3) instead of a release build (-O3), running inside of Visual Studio
or XCode (which are debug builds by default) may all lead to different times.

All samples were run with a command similar to:

time ./logman sample-log.txt < sample-???-cmds.txt > sample-???-out.txt

The times below are the user time reported for each command file (second line: user time):

q: 0.106
d: 0.116
b: 0.114
e: 0.118
t: 0.115
c: 0.121
k: 0.162
all: 1.025
