#define MASTER 0
#define REPETITIONS 10
#define MPI_SENDTYPE MPI_DOUBLE

void DistributeMatrix(T_datatype *root_matrix, T_datatype *local_matrix, int rank, int p, long n, long chunksize){
    int i,j, sendcounts[p], displs[p];
	T_datatype *sendbuffer=NULL;
	long temp;
	MPI_Datatype MPI_coltype, MPI_coltype2; 
	int chunk, lastchunk;
	
	chunk=n/p;
	lastchunk= n - (chunk * ( p - 1) );

	MPI_Type_vector(n, 1, n, MPI_SENDTYPE, &MPI_coltype2);
    MPI_Type_create_resized( MPI_coltype2, 0, sizeof(T_datatype), &MPI_coltype);
	MPI_Type_commit(&MPI_coltype);

	for ( i = 0 ; i < p ; ++i ){
		if ( i == p - 1 )
			sendcounts[i] = lastchunk;
		else
			sendcounts[i] = chunk;
			
		displs[i] = i*chunk;
	}
	
	sendbuffer = NULL;
	if ( rank == MASTER )
		sendbuffer = &(root_matrix[0]);
	
	MPI_Scatterv( sendbuffer, sendcounts, displs, MPI_coltype, &(local_matrix[0]), chunksize*n, MPI_SENDTYPE, MASTER, MPI_COMM_WORLD );		

	MPI_Type_free(&MPI_coltype);
}

void  DistributeVector(T_datatype *x, T_datatype *local_vector, int rank, int p, long n, long chunksize){
	int sendcounts[p], displs[p],i;
	int chunk, lastchunk;
	
	chunk=n/p;
	lastchunk= n - (chunk * ( p - 1) );
	sendcounts[0] = chunk;


	for ( i = 0 ; i < p ; i++ ){
		sendcounts[i] = (i == p - 1 ) ? lastchunk : chunk;
		displs[i] = i*chunk;
	}
	
	if ( rank == MASTER ){
		MPI_Scatterv(&(x[0]), sendcounts, displs, MPI_SENDTYPE, &local_vector[0], chunksize, MPI_SENDTYPE, MASTER, MPI_COMM_WORLD);				
	}else{
		MPI_Scatterv(NULL, sendcounts, displs, MPI_SENDTYPE, &local_vector[0], chunksize, MPI_SENDTYPE, MASTER, MPI_COMM_WORLD);		
	}
	
}

void CalculateAndReduceScatter(T_datatype *A, T_datatype *x, T_datatype *resultvector, int rank, int p, long n, long chunksize){
	T_datatype *result;
	int recvcounts[p];
	int chunk = n / p;
	
	result = initializeResultVector(n, 0.0); //init result vector with 0 
	for ( int i = 0 ; i < p ; ++i ){
		recvcounts[i]=n;
	}
	
	for ( int i = 0 ; i < chunksize ; ++i ){
		for ( int j = 0 ; j < n ; ++j ){
			result[j] = result[j] + A[i*n+j] * x[i];
		}
	}		
	
	MPI_Reduce_scatter(result, resultvector, recvcounts, MPI_SENDTYPE, MPI_SUM, MPI_COMM_WORLD);
	
	free(result);
}

int main(int argc, char* argv[]){
    MPI_Init(&argc, &argv);
    int rank;
    int P,columncount=0;
    long chunksize;
    T_datatype *local_matrix=NULL, *local_vector=NULL;
	T_datatype *resultvector=NULL;
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    
    long n = atol(argv[1]);
    int x[] = genxO(n);
    int A[][] = genA(n, n);

    columncount =  n - ((int)n/P ) * (P - 1 ) ; 
	chunksize = (rank == P - 1 ) ? columncount : ( n / P );
	columncount = ( columncount > chunksize ) ? columncount : chunksize;

    local_matrix = (T_datatype*) malloc (sizeof(T_datatype) * n * columncount);

	local_vector = (T_datatype*) malloc (sizeof(T_datatype) * chunksize );

    resultvector = initializeResultVector(n, 0.0); //init result vector with 0 

    for( i = 0 ; i < REPETITIONS ; ++i ){

		MPI_Barrier(MPI_COMM_WORLD);

        DistributeVector(x, local_vector, rank, P, n, chunksize);
		
		DistributeMatrix(A, local_matrix, rank, P, n, chunksize);

		CalculateAndReduceScatter(local_matrix, local_vector, resultvector, rank, P, n, chunksize);
		
		MPI_Barrier(MPI_COMM_WORLD); 

		MPI_Reduce(&time, &slowest, 1, MPI_DOUBLE, MPI_MAX, MASTER, MPI_COMM_WORLD);
    }
     
    free(A);
    free(x);
    free(local_matrix);
    free(local_vector);
    free(resultvector);
    MPI_Finalize();

    return 0;
}