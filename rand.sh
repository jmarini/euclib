#!/usr/bin/env bash
# Simple script to repeatedly run the program (generating random data each time)
#   and calling gnuplot to see the result.

for ((;;))
do
	./test
	make plot
done

