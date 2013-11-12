
SYSTEM     = x86-64_sles10_4.1
LIBFORMAT  = static_pic

#------------------------------------------------------------
#
# When you adapt this makefile to compile your CPLEX programs
# please copy this makefile and set CPLEXDIR and CONCERTDIR to
# the directories where CPLEX and CONCERT are installed.
#
#------------------------------------------------------------

CPLEXDIR      = /opt/ibm/ILOG/CPLEX_Studio124/cplex
CONCERTDIR    = /opt/ibm/ILOG/CPLEX_Studio124/concert
# ---------------------------------------------------------------------
# Compiler selection 
# ---------------------------------------------------------------------

CCC = g++ -O0
CC  = gcc -O0

# ---------------------------------------------------------------------
# Compiler options 
# ---------------------------------------------------------------------

CCOPT = -m64 -O -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD

# ---------------------------------------------------------------------
# Link options and libraries
# ---------------------------------------------------------------------

CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

CCLNFLAGS = -L$(CPLEXLIBDIR) -lilocplex -lcplex -L$(CONCERTLIBDIR) -lconcert -lm -pthread

all:
	make all_cpp

execute: all
	make execute_c
	make execute_cpp
	make execute_java
CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include

EXDIR         = .
EXINC         = $(EXDIR)
EXDATA        = $(EXDIR)
EXSRCC        = $(EXDIR)
EXSRCCX       = $(EXDIR)
EXSRCCPP      = $(EXDIR)


CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) 



#------------------------------------------------------------
#  make all      : to compile the examples. 
#  make execute  : to compile and execute the examples.
#------------------------------------------------------------


CPP_EX = conj

all_cpp: $(CPP_EX)

execute_cpp: $(CPP_EX)
	 ./conj

conj: conj.o
	$(CCC) $(CCFLAGS) conj.o -o conj $(CCLNFLAGS)
conj.o: $(EXSRCCPP)/conjCplex.cpp
	$(CCC) -c $(CCFLAGS) $(EXSRCCPP)/conjCplex.cpp -o conj.o

clean:
	rm conj.o
	rm conj
	rm conjunto.lp
	rm conjunto.sol
