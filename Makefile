CMPL = g++
FLGS = -Wall -Wextra -pedantic -std=c++0x
DFLG = -g
RFLG = -O3
PROG = test
PLOT = plot.out
LIBS = 
SRCS = main.cpp

all:
	$(CMPL) $(FLGS) $(DFLG) -o $(PROG) $(SRCS) $(LIBS)
	
release:
	$(CMPL) $(FLGS) $(RFLG) -o $(PROG) $(SRCS) $(LIBS)

debug:
	$(CMPL) $(FLGS) $(DFLG) -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm -f $(PLOT) $(PROG)

plot: $(PROG)
	gnuplot $(PLOT)		

check:
	cppcheck --enable=all .

