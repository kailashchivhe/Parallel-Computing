CFLAGS=-O3 -std=c11 -fPIC -g -fopenmp
CXXFLAGS=-O3 -std=c++11 -fPIC -g -fopenmp
LDFLAGS=-fopenmp
ARCHIVES=../libgen.a 
LD=g++


UTILDIR=util


all: reduce_seq mergesort_seq


# archives

libgen.a: gen_lib.o
	ar rcs libgen.a gen_lib.o



reduce_seq: reduce_seq.o
	$(LD) $(LDFLAGS) reduce_seq.o $(ARCHIVES) -o reduce_seq


mergesort_seq: mergesort_seq.o
	$(LD) $(LDFLAGS) mergesort_seq.o $(ARCHIVES) -o mergesort_seq


# run benchmarks
bench: reduce_seq mergesort_seq
	./queue.sh

clean:
	-rm *.o
	-rm reduce_seq mergesort_seq

distclean:
	-rm *.sh.*


.PHONY: reduce_seq mergesort_seq
