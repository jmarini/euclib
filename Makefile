CMPL = g++
FLGS = -Wall -Wextra -pedantic -g -std=c++0x
PROG = test
PLOT = plot.out
LIBS = 
SRCS = main.cpp

all:
	$(CMPL) $(FLGS) -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm -f $(PLOT) $(PROG)

plot: $(PROG)
	gnuplot $(PLOT)		

