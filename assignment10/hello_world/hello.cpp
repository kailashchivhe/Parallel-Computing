#include <mpi.h>
#include <unistd.h>
#include <iostream>

int main(int argc, char*argv[]) {
    int id, p, size;
    char machine_name[100];
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Get_processor_name(machine_name,&size);
    printf("I am Process %d out of %d. Running on %s\n", id, p, machine_name );
    MPI_Finalize();
    return 0;
}
