#include <mpi.h>
#include <unistd.h>
#include <iostream>

int main(int argc, char*argv[]) {
    int i, id, p;
    int solutions, global_solutions;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    for (i = id; i < 65535; i += p)
    solutoins += check_circuit(id, i);
    MPI_Reduce(&solutions, &global_solutions, 1, MPI_INT, MPI_SUM,
    0, MPI_COMM_WORLD);
    printf("Process %d is done\n", id);
    MPI_Finalize();
    return 0;
}
