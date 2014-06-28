/* Universidad Nacional de La Matanza */
/* Sistemas Operativos Avanzados */
/* TP 2 - Computacion de Alto Rendimiento */
/* Algoritmo de Multiplicacion de Matrices */
/* Basado en codigo provisto por el Prof. Fernando Tinetti (UNLP) */

#include <string.h>
#include <stdio.h>
#include <stdlib.h> 
#include <sys/time.h>

float *A, *B, *C;
float *C_p;
int n, b;
int n_bar, b_sqr;


void initvalmat(float *mat, int n, float val); 
void verify_result(float *c, int n);
double dwalltime();
void printMatrix(float * M, int n);

void Blocked_mat_mult(void);
void Zero_C(int i_bar, int j_bar);
void Mult_add(int i_bar, int j_bar, int k_bar);


main(int argc, char *argv[])
{
  double timetick;

  /* Chequeando parametros */
  if ((argc != 3) || ((n = atoi(argv[1])) <= 0) || ((b = atoi(argv[2])) <= 0) )
  {
    printf("Uso: %s n b\n  donde n: dimension de la matriz cuadrada (nxn X nxn) y b el tamao del bloque\n", argv[0]);
    exit(1);
  }

  // Inicializando Matrices
  A = (float *) malloc(n*n*sizeof(float));
  B = (float *) malloc(n*n*sizeof(float));
  C = (float *) malloc(n*n*sizeof(float));
  
  initvalmat(A, n, 1.0); 
  initvalmat(B, n, 1.0); 
  initvalmat(C, n, 0.0); 
  
  n_bar = n/b;
  b_sqr = b*b;

  printf("Multiplicando matrices de %d x %d en bloques de %d \n", n, n, b);  
  timetick = dwalltime();
  
  /*************************************************************/
  /* Programar aqui el algoritmo de multiplicacion de matrices */
  /*************************************************************/
  
  Blocked_mat_mult();
  
  /*************************************************************/
  
  timetick = dwalltime() - timetick;
  verify_result(C, n);

  printf("Resultado correcto. Tiempo de ejecucion: %f segundos\n", timetick);
}

void Zero_C(int i_bar, int j_bar) {
   C_p = C + (i_bar*n_bar + j_bar)*b_sqr;

   memset(C_p, 0, b_sqr*sizeof(float));
}

void Blocked_mat_mult(void){
   int i_bar, j_bar, k_bar;  // index block rows and columns

   for (i_bar = 0; i_bar < n_bar; i_bar++)
      for (j_bar = 0; j_bar < n_bar; j_bar++) {
         Zero_C(i_bar, j_bar);
         for (k_bar = 0; k_bar < n_bar; k_bar++) 
            Mult_add(i_bar, j_bar, k_bar);
      }
}

void Mult_add(int i_bar, int j_bar, int k_bar) {
   float *c_p = C_p;
   float *a_p = A + (i_bar*n_bar + k_bar)*b_sqr;
   float *b_p = B + (k_bar*n_bar + j_bar)*b_sqr;
   int i, j, k;

   for (i = 0; i < b; i++)
      for (j = 0; j < b; j++) 
         for (k = 0; k < b; k++)
            *(c_p + i*b + j) += 
               (*(a_p + i*b+k))*(*(b_p + k*b + j));
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
      if (c[i*n + j] != n)
      {
        printf("Error en %d, %d, valor: %f\n", i, j, c[i*n + j]);
		exit(-1);
      }
    }
  }
}

