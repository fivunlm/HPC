/* Universidad Nacional de La Matanza */
/* Sistemas Operativos Avanzados */
/* TP 2 - Computacion de Alto Rendimiento */
/* Algoritmo de Multiplicacion de Matrices */
/* Basado en codigo provisto por el Prof. Fernando Tinetti (UNLP) */

#include <stdio.h>
#include <stdlib.h> 
#include <sys/time.h>
#include <math.h>
#include <mpi.h>

void initvalmat(float *mat, int n, float val); 
void verify_result(float *c, int n);
double dwalltime();
float matrix_get_cell(float *matrix,int n, int x, int y);
int matrix_set_cell(float *matrix,int n, int x, int y, float val);
void printMatrix(float * M, int n);

main(int argc, char *argv[])
	{
	float *A, *B, *C;
	int n, i, j, k, y ,x, iPORn, inMASj;
	//double timetick;
	double timetick1,timetick2;

	int completerows;
	int *resultrow;
	float elem1, elem2, suma;
	int partitions, firstrow, lastrow, rowstodo;

	int rank;
	MPI_Status status;
	int count;
	int rc;

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &count);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	//printf("rank: %d\n", rank);
	/* Chequeando parametros */
	if ((argc != 2) || ((n = atoi(argv[1])) <= 0) )
		{
		printf("Uso: %s n\n  donde n: dimension de la matriz cuadrada (nxn X nxn)\n", argv[0]);
		exit(1);
		}

	// Inicializando Matrices
	A = (float *) malloc(n*n*sizeof(float));
	B = (float *) malloc(n*n*sizeof(float));
	C = (float *) malloc(n*n*sizeof(float));
	initvalmat(A, n, 1.0); 
	initvalmat(B, n, 1.0); 
	initvalmat(C, n, 0.0);	

/* En MPI el proceso inicial (padre) tiene rank
de 0, esta es la sección donde el proceso padre
realiza su trabajo */
if (rank == 0) 
	{
	//printf("rank: %d Soy rank 0\n", rank);
	/*Determinar el momento de inicio de ejecución,
	aquí medimos cuando comenzamos realmente a
	hacer los cálculos */
	timetick1 = MPI_Wtime();

	// mi registro de cuales rows ya estan completos
	completerows = 0;

	/* el proceso con rank 0 transmite las dos
	matrices a los demás, esto se hace por medio
	de un broadcast de MPI a todos los miembros de
	mi comunicador (MPI_COMM_WORLD) */
	MPI_Bcast(A,n * n,	MPI_FLOAT, 0, MPI_COMM_WORLD);
	MPI_Bcast(B,n * n, MPI_FLOAT, 0, MPI_COMM_WORLD);
		
	// asignar un row temporal para resultados
	resultrow = malloc((n + 1) * sizeof(float));

	// esperar a tener todos los resultados completos
	while (completerows < n) 
	{
		/* Esta llamada espera a recibir un renglón
		ya resuelto, de cualquier proceso
		hijo. Nótese el parametro MPI_ANY_SOURCE
		que indica que se admiten valores de
		cualquier proceso. */
		MPI_Recv(resultrow,n + 1,MPI_INT,MPI_ANY_SOURCE,1, MPI_COMM_WORLD, &status);
		
		//printf("rank0: recibido renglon %d de %d\n",resultrow[0], status.MPI_SOURCE);
		completerows++;
		// Este ciclo "pone" el renglon recibido en mi matriz de resultado
		for (i = 0; i < n; i++) 
			{
			matrix_set_cell(C,n,i,resultrow[0],resultrow[i + 1]);
			}
		}
		// terminamos el cálculo! anotar tiempo al terminar..
		timetick2 = MPI_Wtime();
		//printMatrix(C,n);
		verify_result(C, n);
		printf("Resultado correcto. Tiempo de ejecucion: %f segundos\n", (timetick2 - timetick1) );
	} // AQUI termina la ejecucion del proceso padre
else 
	{
	/* En MPI, si mi rank no es 0, una llamada al
	broadcast (notar el parametro 4 que es de 0)
	indica recibir el broadcast del proceso con ese
	rank. */
	MPI_Bcast(A,n * n,MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(B,n * n,MPI_INT, 0, MPI_COMM_WORLD);

	/*de acuerdo a las dimensiones de las matrices
	y al numero de procesos, calcular numero de
	particiones, renglones por particion, el
	primer renglon que tiene que resolver este
	proceso, y el ultimo renglon.  */
	partitions = count - 1;
	rowstodo = ceil ((float)n / partitions);
	firstrow = rowstodo * (rank - 1);
	lastrow = firstrow + rowstodo - 1;
	lastrow = lastrow <= n ? lastrow : n ;	

	//printf("rank-%d: partitions: %d, rowstodo: %d %.2f, firstrow: %d, lastrow: %d\n", rank, partitions, rowstodo, firstrow, lastrow);
	// asignar un row temporal
	resultrow = malloc((n + 1) * sizeof(float));

	// calcular cada row del grupo que me toca.
	for (i = firstrow; i <= lastrow; i++) 
		{
		/* printf ("proceso %d[%s] haciendo row
		%d\n",localid,processor_name,i); */
		/* el elemento que estoy calculando esta en
		y,x entonces lo que va a variar va a ser
		la x porque la y es fija por renglon */
		y = i;
		for (x = 0; x < n; x++) 
			{
			//printf ("calculando elemento (%d,%d)\n",x,y);
			suma = 0;
			for (k = 0; k < n; k++) 
				{
				elem1 = matrix_get_cell(A,n,k,y);
				elem2 = matrix_get_cell(B,n,x,k);
				suma += elem1 * elem2;
				//printf ("%f*%f +",elem1,elem2);
				}
			resultrow[x + 1] = suma;
			}
		// ya tengo calculado el renglon ahora se
		// lo tengo que mandar al proceso padre
		resultrow[0] = i;
		MPI_Send(resultrow,n + 1,MPI_INT, 0, 1, MPI_COMM_WORLD);
		//printf ("enviado row %d\n",i);
		}
	} // termina seccion de calculo proceso hijo

	MPI_Finalize();

	
	}

// Funciones Auxiliares

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
			if (c[i*n + j] != n)
				{
				printf("Error en %d, %d, valor: %f\n", i, j, c[i*n + j]);
				}
			}
		}
	}

float matrix_get_cell(float *matrix,int n, int x, int y)
	{
    int valor_lineal;
    valor_lineal = (y * n + x);
    return matrix[valor_lineal];
	}

int matrix_set_cell(float *matrix,int n, int x, int y, float val)
	{
    int valor_lineal;
    valor_lineal = (y * n + x);
    matrix[valor_lineal] = val;
    return 0;
	}


