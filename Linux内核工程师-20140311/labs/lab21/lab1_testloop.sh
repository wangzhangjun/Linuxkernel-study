#!/bin/bash

P=./lab21-1

# Take your choice of looping methods:

# n=19
# while [ $n -ge -20 ] ; do ($P $n &) ;  n=$((n-4)) ; done

for n in 19 15 10 5 0 -5 10 -15 -20 ; do ($P $n &) ; done


