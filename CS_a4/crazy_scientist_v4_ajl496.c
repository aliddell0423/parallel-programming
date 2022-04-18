#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>

#define SIZE 50

int t1_iterations = 0;
int t2_iterations = 0;
double mat[SIZE][SIZE];
int rows[SIZE];
double t1start;
double t2start;
double t1end;
double t2end;

double do_crazy_computation(int i,int j);
void *do_work1();
void *do_work2();

int main(int argc, char **argv) {

  pthread_t thread1;
  pthread_t thread2;

  int i, j;
  for(i=0; i<SIZE; i++)
    {
      rows[i] = 0;
    }
  pthread_create( &thread1, NULL, do_work1, NULL);
  pthread_create( &thread2, NULL, do_work2, NULL);

  pthread_join( thread1, NULL );
  pthread_join( thread2, NULL );

  double t1time = t1end - t1start;
  double t2time = t2end - t2start;
  double loadimbalance = fabs(t1time - t2time);

  printf("Number of iterations for thread 1: %d\n", t1_iterations );
  printf("Number of iterations for thread 2: %d\n", t2_iterations );

  printf("Time thread 1: %.3f\n", t1time);
  printf("Time thread 2: %.3f\n", t2time);

  printf("Load imbalance: %.3f\n", loadimbalance );

  exit(0);
}

//Crazy computation
double do_crazy_computation(int x,int y) {
   int iter;
   double value=0.0;

   for (iter = 0; iter < 5*x*x*x+1 + y*y*y+1; iter++) {
     value +=  (cos(x*value) + sin(y*value));
   }
  return value;
}


void *do_work1()
  {
    t1start = omp_get_wtime();
    int i, j;
    for (i=0;i<SIZE;i++) { /* loop over the rows */
      if( rows[i] == 0 )
        {
          rows[i] = 1;
          for (j=0;j<SIZE;j++) {  /* loop over the columns */
            mat[i][j] = do_crazy_computation(i,j);
            // fprintf(stderr,".");
          }
          t1_iterations += 1;
        }
    }
    t1end = omp_get_wtime();
    return NULL;
  }

void *do_work2()
  {
    t2start = omp_get_wtime();
    int i, j;
    for (i=0;i<SIZE;i++) { /* loop over the rows */
      if( rows[i] == 0 )
        {
          rows[i] = 1;
          for (j=0;j<SIZE;j++) {  /* loop over the columns */
            mat[i][j] = do_crazy_computation(i,j);
            // fprintf(stderr,".");
          }
          t2_iterations += 1;
        }
    }
    t2end = omp_get_wtime();
    return NULL;
  }
