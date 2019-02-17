# 2019

A solver for the math problem presented at http://mathforum.org/yeargames

This exhaustively searches (with some simple pruning) for equations using only
the digits 2, 0, 1 and 9 which generate integral answers between 1 and 100.

It is configured to build using bazel, using "bazel build -c opt 2019".  Or you
use your favorite build environment.

This is far from optimized.  It needs about 11GB of RAM to complete, and the
code does some pretty stupid things.  I was optimizing for my time, not yours.
;-)

When I run it, it finds solutions for all values from 1 to 100, generating a
total of 6526951 equations.
