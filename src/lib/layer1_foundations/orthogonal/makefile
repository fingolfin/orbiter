DISCRETA_CPP = g++
DISCRETA_CC = gcc
CPPFLAGS = -Wall -O3 -funroll-loops -I.. -std=c++14 
# -frename-registers
LIB = 
LFLAGS = -lm

MODULES = \
	blt_set_domain.o \
	blt_set_invariants.o \
	hyperbolic_pair_hyperbolic.o \
	hyperbolic_pair_parabolic.o \
	hyperbolic_pair_rank_unrank.o \
	hyperbolic_pair.o \
	orthogonal_global.o \
	orthogonal_group.o \
	orthogonal_indexing.o \
	orthogonal_io.o \
	orthogonal.o \
	unusual_model.o



all : $(MODULES) 

%.o: %.cpp %.c; $(DISCRETA_CPP) $(CPPFLAGS) -c  -o $@ $<

clean:
	- rm *.o *.a *~

