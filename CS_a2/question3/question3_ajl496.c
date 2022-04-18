#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct arguments {
  int *total;
  int *current_iteration;
  int value;
};

void *do_work( void *arg );

int main( int argc, char **argv ) {

  pthread_t worker_thread[10];
  struct arguments *arg[10];

  int current_iteration = 0;
  int total = 0;

  for(int i = 0; i < 10; i = i + 1)
    {
      // Build argument to threads
      arg[i] = ( struct arguments * )calloc( 1, sizeof( struct arguments ) );
      arg[i]->value= i;
      arg[i]->current_iteration = &current_iteration;
      arg[i]->total = &total;

      // Create thread
      if ( pthread_create( &worker_thread[i], NULL,
                           do_work, ( void * )arg[i])) {
        fprintf( stderr,"Error while creating thread\n" );
        exit( 1 );

      }
    }

  for(int i = 0; i < 10; i = i + 1)
     { // Join with thread
       if ( pthread_join( worker_thread[i], NULL ) ) {
          fprintf( stderr,"Error while joining with child thread\n" );
          exit( 1 );
       }
     }

  printf( "Total: %d", total );

  exit( 0 );

  return 0;
}

void *do_work( void *arg ) {

  struct arguments *argument;
  argument = ( struct arguments *) arg;

  int *current_iteration = argument->current_iteration;
  int *total = argument->total;
  int value = argument->value;

  while( *total < 990 )
    {
      while( value != *current_iteration % 10 && *total < 990 );

      if( *total < 990 )
        {
          *total = *total + value;

          printf( "My num: %d, total: %d\n", value, *total );

          *current_iteration = *current_iteration + 1;
        }
    }

  return NULL;
}
