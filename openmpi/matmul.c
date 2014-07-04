/* Universidad Nacional de La Matanza */
/* Sistemas Operativos Avanzados */
/* TP 2 - Computacion de Alto Rendimiento */
/* Algoritmo de Multiplicacion de Matrices */
/* Basado en codigo provisto por el Prof. Fernando Tinetti (UNLP) */

#include <stdio.h>
#include <stdlib.h> 
#include <sys/time.h>
#include <mpi.h>

void initvalmat(float *mat, int n, float val); 
void verify_result(float *c, int n);
double dwalltime();
void printMatrix(float * M, int n);

main(int argc, char *argv[])
	{
	float *A, *B, *C;
	int n, i, j, k, iPORn, inMASj;
	//double timetick;
	double timetick1,timetick2;

	int rank;
	MPI_Status Stat;
	int count;
	int rc;

	int ComTask;//
	int FinTask;//
	int CantPorTask;//

	rc = MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &count);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	/* Chequeando parametros */
	if ((argc != 2) || ((n = atoi(argv[1])) <= 0) )
	{
		printf("Uso: %s n\n  donde n: dimension de la matriz cuadrada (nxn X nxn)\n", argv[0]);
		exit(1);
	}

	A = (float *) malloc(n*n*sizeof(float));
	B = (float *) malloc(n*n*sizeof(float));
	C = (float *) malloc(n*n*sizeof(float));

	if (rank == 0) 
	{
		// Inicializando Matrices
		initvalmat(A, n, 1.0); 
		initvalmat(B, n, 1.0); 
		initvalmat(C, n, 0.0);
		printf("Multiplicando matrices de %d x %d \n", n, n);
	}
  
	//timetick = dwalltime();
	timetick1 =  MPI_Wtime();

	MPI_Bcast(A, n * n, MPI_FLOAT, 0, MPI_COMM_WORLD);
	MPI_Bcast(B, n * n, MPI_FLOAT, 0, MPI_COMM_WORLD);
	MPI_Bcast(C, n * n, MPI_FLOAT, 0, MPI_COMM_WORLD);

	CantPorTask = n / count;
	ComTask = CantPorTask * rank;
	FinTask = CantPorTask * (rank + 1);

	/*************************************************************/
	/* Programar aqui el algoritmo de multiplicacion de matrices */
	/*************************************************************/

 	for( i = ComTask; i < FinTask; ++i)
 	{	
    	iPORn = n * i;
      	for( j = 0; j < n ; ++j ) 
      	{	         
      		inMASj = iPORn+j;
        	for( k = 0; k < n; ++k )
         	{                   
           		C[inMASj] += A[iPORn+k]*B[n*k+j];	
         	} 
      	}
  	}

 
	/*************************************************************/

	MPI_Gather( &C[n*ComTask], n*(FinTask - ComTask), MPI_FLOAT, &C[n*ComTask], n*(FinTask - ComTask), MPI_FLOAT, 0, MPI_COMM_WORLD); 

	timetick2 = MPI_Wtime();

	if (rank == 0)
	{
		verify_result(C, n);
		printf("Resultado correcto. Tiempo de ejecucion: %f segundos\n", (timetick2 - timetick1) );
	}	

	MPI_Finalize();

	}


// Funciones Auxiliares

/* Init square matrix with a specific value */
void initvalmat(float *mat, int n, float val)
	{
	int i, j;      /* Indexes */
	for (i = 0; i < n; i++)
		{
		for (j = 0; j < n; j++)
			{
			mat[i*n + j] = val;
			}
		}
	}

double dwalltime()
	{
	double sec;
	struct timeval tv;

	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
	}

void verify_result(float *c, int n)
	{
	int i, j;
	/* Check results (just in case...) */
	for (i = 0; i < n; i++)
		{
		for (j = 0; j < n; j++)
			{
	//printf("%f\n",c[i*n + j]);
			if (c[i*n + j] != n)
				{
				printf("Error en %d, %d, valor: %f\n", i, j, c[i*n + j]);
				MPI_Finalize();
				exit(-1);
				}
		
			}
		}
	}

void printMatrix(float * M, int n)
{
  int i, j, k, iPORn, inMASj;

  for (i = 0; i < n; i++)
  {
    iPORn=i*n;
    printf("| ");
    for (j = 0; j < n; j++)
    {
      printf("%.2f ", M[iPORn+j]);
    }
    printf(" |\n");
  }  
}

