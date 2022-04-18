#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>

#define SIZE 50

double do_crazy_computation(int i,int j);

int main(int argc, char **argv) {
  double mat[SIZE][SIZE];
  int i,j;

  double tstart=omp_get_wtime();

  //modify code here

  omp_set_num_threads(2);

  double t_start;
  double t_end;
  double thread_1;

  #pragma omp parallel private(t_end, t_start)
  {
    t_start = omp_get_wtime();

    #pragma omp for nowait private(i)
    for (i=0;i<SIZE;i++) { /* loop over the rows */
      for (j=0;j<SIZE;j++) {  /* loop over the columns */
        mat[i][j] = do_crazy_computation(i,j);
        // fprintf(stderr,".");
      }
    }
    t_end = omp_get_wtime();

    double t_elapsed= t_end - t_start;
    printf("Time thread %d: %f seconds\n", omp_get_thread_num(), t_elapsed);

    if( omp_get_thread_num() == 0 )
      {
        thread_1 = t_elapsed;
      }

  }

  double tend=omp_get_wtime();
  double elapsed=tend - tstart;
  printf("Elapsed time: %f seconds\n",elapsed);
  printf("Load Imbalance: %f seconds\n",elapsed - thread_1);

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
