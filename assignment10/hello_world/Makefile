CXX=mpicxx
CC=mpicxx
LD=mpicxx

hello:

run_1x16: hello
	sbatch --partition=Centaurus --time=00:05:00 --nodes=1 --ntasks-per-node=16 ./run.sh

run_2x8: hello
	sbatch --partition=Centaurus --time=00:05:00 --nodes=2 --ntasks-per-node=8 ./run.sh

run_4x8: hello
	sbatch --partition=Centaurus --time=00:05:00 --nodes=4 --ntasks-per-node=8 ./run.sh

run: run_1x16 run_2x8 run_4x8

clean:
	-rm *.o
	-rm hello

distclean:
	rm *.sh.*
