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

  #pragma omp parallel for private(i) schedule(static)

  for (i=0;i<SIZE;i++) { /* loop over the rows */
    for (j=0;j<SIZE;j++) {  /* loop over the columns */
      mat[i][j] = do_crazy_computation(i,j);
      // fprintf(stderr,".");
    }
  }

  double tend=omp_get_wtime();
  double elapsed=tend - tstart;
  printf("Elapsed time: %f seconds\n",elapsed);

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
